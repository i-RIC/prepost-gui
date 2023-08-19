#include "../post2dwindowgraphicsview.h"
#include "post2dwindowgridtypedataitem.h"
#include "post2dwindowparticlesbasescalargroupdataitem.h"
#include "post2dwindowparticlesbasetopdataitem.h"
#include "post2dwindowparticlesbasevectorgroupdataitem.h"
#include "post2dwindowparticlesbasevectorgrouptopdataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindowparticlesbasevectorgroupdataitem_settingeditwidget.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guicore/arrows/arrowssettingtoolbarwidget.h>
#include <guicore/datamodel/graphicswindowdataitemupdateactorsettingdialog.h>
#include <guicore/misc/targeted/targeteditemsettargetcommandtool.h>
#include <guicore/named/namedgraphicswindowdataitemtool.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/scalarstocolors/colormapsettingcontainer.h>
#include <guicore/scalarstocolors/colormapsettingmodifycommand.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <guicore/solverdef/solverdefinitiongridoutput.h>
#include <misc/iricundostack.h>
#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>
#include <misc/stringtool.h>
#include <misc/valuemodifycommandt.h>

Post2dWindowParticlesBaseVectorGroupDataItem::Post2dWindowParticlesBaseVectorGroupDataItem(const std::string& target, Post2dWindowDataItem* p) :
	Post2dWindowDataItem("", QIcon(":/libs/guibase/images/iconPaper.svg"), p),
	m_actor {vtkActor::New()},
	m_legendActor {vtkActor2D::New()},
	m_arrowsToolBarWidget {new ArrowsSettingToolBarWidget(mainWindow())}
{
	setupStandardItem(NotChecked, NotReorderable, NotDeletable);

	auto cont = zoneDataItem()->dataContainer();
	SolverDefinitionGridType* gt = cont->gridType();
	auto caption = gt->vectorOutputCaption(target);

	m_setting.arrowsSetting.target = target.c_str();
	m_standardItem->setText(caption);
	m_setting.arrowsSetting.legend.title = caption;

	m_setting.arrowsSetting.legend.imageSetting.setActor(m_legendActor);
	m_setting.arrowsSetting.legend.imageSetting.controller()->setItem(this);

	for (auto name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(topDataItem()->particleData()->GetPointData())) {
		m_setting.arrowsSetting.colorTarget = name.c_str();
		break;
	}

	m_arrowsToolBarWidget->hide();
	m_arrowsToolBarWidget->setColorMapSettings(topDataItem()->scalarGroupDataItem()->colorMapSettings());
	m_arrowsToolBarWidget->setSetting(&m_setting.arrowsSetting);

	connect(m_arrowsToolBarWidget, &ArrowsSettingToolBarWidget::updated, [=]() {
		auto com = new MergeSupportedListCommand(iRIC::generateCommandId("ArrowModify"), false);
		auto newSetting = m_arrowsToolBarWidget->modifiedSetting();
		com->addCommand(new ValueModifyCommmand<ArrowsSettingContainer>(iRIC::generateCommandId("ArrowsSetting"), false, newSetting, &m_setting.arrowsSetting));

		if (newSetting.colorMode == ArrowsSettingContainer::ColorMode::ByScalar) {
			auto cm = topDataItem()->scalarGroupDataItem()->colorMapSettings().at(iRIC::toStr(newSetting.colorTarget));
			com->addCommand(new ColorMapSettingModifyCommand(m_arrowsToolBarWidget->modifiedColorMapSetting(), cm));
		}
		pushUpdateActorSettingCommand(com, this);
	});

	setupActors();
	updateCheckState();
}

Post2dWindowParticlesBaseVectorGroupDataItem::~Post2dWindowParticlesBaseVectorGroupDataItem()
{
	auto r = renderer();
	r->RemoveActor(m_actor);
	r->RemoveActor2D(m_legendActor);

	m_actor->Delete();
	m_legendActor->Delete();
}

std::string Post2dWindowParticlesBaseVectorGroupDataItem::target() const
{
	return m_setting.arrowsSetting.target;
}

void Post2dWindowParticlesBaseVectorGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(1);
}

void Post2dWindowParticlesBaseVectorGroupDataItem::assignActorZValues(const ZDepthRange& range)
{
	m_actor->SetPosition(0, 0, range.max());
}

void Post2dWindowParticlesBaseVectorGroupDataItem::update()
{
	updateActorSetting();
}

void Post2dWindowParticlesBaseVectorGroupDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

QDialog* Post2dWindowParticlesBaseVectorGroupDataItem::propertyDialog(QWidget* p)
{
	auto dialog = new GraphicsWindowDataItemUpdateActorSettingDialog(this, p);
	auto widget = new SettingEditWidget(this, dialog);
	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Particles Vector Setting (%1)").arg(m_standardItem->text()));
	dialog->resize(900, 650);

	return dialog;
}

void Post2dWindowParticlesBaseVectorGroupDataItem::handleStandardItemChange()
{
	GraphicsWindowDataItem::handleStandardItemChange();
	topDataItem()->updateColorMapLegendsVisibility();
}

void Post2dWindowParticlesBaseVectorGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);
	updateCheckState();
	updateActorSetting();
}

void Post2dWindowParticlesBaseVectorGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);
}

void Post2dWindowParticlesBaseVectorGroupDataItem::innerUpdate2Ds()
{
	updateActorSetting();
}

void Post2dWindowParticlesBaseVectorGroupDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	zoneDataItem()->initParticleResultAttributeBrowser(particleData());
}

void Post2dWindowParticlesBaseVectorGroupDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	zoneDataItem()->clearParticleResultAttributeBrowser();
}

void Post2dWindowParticlesBaseVectorGroupDataItem::doHandleResize(QResizeEvent* event, VTKGraphicsView* v)
{
	auto& as = m_setting.arrowsSetting;
	as.legend.imageSetting.controller()->handleResize(event, v);

	auto cs = activeColorMapSetting();
	if (cs != nullptr) {
		cs->legendSetting()->imgSetting()->controller()->handleResize(event, v);
	}
}

void Post2dWindowParticlesBaseVectorGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	zoneDataItem()->updateParticleResultAttributeBrowser(event->pos(), v);
	std::vector<ImageSettingContainer::Controller*> controllers;

	auto& as = m_setting.arrowsSetting;
	as.legend.imageSetting.controller()->handleMouseMoveEvent(event, v, true);
	controllers.push_back(as.legend.imageSetting.controller());

	auto cs = activeColorMapSettingWithVisibleLegend();
	if (cs != nullptr) {
		cs->legendSetting()->imgSetting()->controller()->handleMouseMoveEvent(this, event, v, true);
		controllers.push_back(cs->legendSetting()->imgSetting()->controller());
	}

	ImageSettingContainer::Controller::updateMouseCursor(v, controllers);
}

void Post2dWindowParticlesBaseVectorGroupDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	std::vector<ImageSettingContainer::Controller*> controllers;

	auto& as = m_setting.arrowsSetting;
	as.legend.imageSetting.controller()->handleMousePressEvent(event, v, true);
	controllers.push_back(as.legend.imageSetting.controller());

	auto cs = activeColorMapSettingWithVisibleLegend();
	if (cs != nullptr) {
		cs->legendSetting()->imgSetting()->controller()->handleMousePressEvent(this, event, v, true);
		controllers.push_back(cs->legendSetting()->imgSetting()->controller());
	}

	ImageSettingContainer::Controller::updateMouseCursor(v, controllers);
}

void Post2dWindowParticlesBaseVectorGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	zoneDataItem()->fixParticleResultAttributeBrowser(event->pos(), v);
	std::vector<ImageSettingContainer::Controller*> controllers;

	auto& as = m_setting.arrowsSetting;
	as.legend.imageSetting.controller()->handleMouseReleaseEvent(event, v, true);
	controllers.push_back(as.legend.imageSetting.controller());

	auto cs = activeColorMapSettingWithVisibleLegend();
	if (cs != nullptr) {
		cs->legendSetting()->imgSetting()->controller()->handleMouseReleaseEvent(this, event, v, true);
		controllers.push_back(cs->legendSetting()->imgSetting()->controller());
	}

	ImageSettingContainer::Controller::updateMouseCursor(v, controllers);
}

void Post2dWindowParticlesBaseVectorGroupDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(topDataItem()->showAttributeBrowserAction());
}

bool Post2dWindowParticlesBaseVectorGroupDataItem::addToolBarButtons(QToolBar* toolBar)
{
	m_arrowsToolBarWidget->setParent(toolBar);
	m_arrowsToolBarWidget->show();

	toolBar->addWidget(m_arrowsToolBarWidget);

	return true;
}

void Post2dWindowParticlesBaseVectorGroupDataItem::setupActors()
{
	m_actor->GetProperty()->SetLighting(false);

	auto r = renderer();
	r->AddActor(m_actor);
	r->AddActor2D(m_legendActor);
}

void Post2dWindowParticlesBaseVectorGroupDataItem::updateCheckState()
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(m_setting.arrowsSetting.target, m_childItems, true);
}

void Post2dWindowParticlesBaseVectorGroupDataItem::updateActorSetting()
{
	m_actor->VisibilityOff();
	m_legendActor->VisibilityOff();
	actorCollection()->RemoveAllItems();
	actor2DCollection()->RemoveAllItems();

	auto data = particleData();
	if (data == nullptr) {return;}
	if (m_setting.arrowsSetting.target == "") {
		topDataItem()->updateColorMapLegendsVisibility();
		return;
	}

	m_setting.arrowsSetting.updateStandardValueIfNeeded(data->GetPointData());
	auto filteredData = m_setting.arrowsSetting.buildFilteredData(data);

	auto sampledData = m_setting.filteringSetting.buildSampledData(filteredData);
	filteredData->Delete();

	auto view = dataModel()->graphicsView();
	auto arrowsData = m_setting.arrowsSetting.buildArrowsPolygonData(sampledData, view);
	sampledData->Delete();

	if (m_setting.arrowsSetting.colorMode == ArrowsSettingContainer::ColorMode::Custom) {
		auto mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOff();
		auto gfilter = vtkSmartPointer<vtkGeometryFilter>::New();
		gfilter->SetInputData(arrowsData);
		mapper->SetInputConnection(gfilter->GetOutputPort());
		m_actor->SetMapper(mapper);
		mapper->Delete();

		m_actor->GetProperty()->SetColor(m_setting.arrowsSetting.customColor);
	} else if (m_setting.arrowsSetting.colorMode == ArrowsSettingContainer::ColorMode::ByScalar) {
		arrowsData->GetPointData()->SetActiveScalars(iRIC::toStr(m_setting.arrowsSetting.colorTarget).c_str());
		auto cs = activeColorMapSetting();
		if (cs == nullptr) {return;}
		auto mapper = cs->buildPointDataMapper(arrowsData);
		m_actor->SetMapper(mapper);
		mapper->Delete();
	}
	arrowsData->Delete();

	auto v = dataModel()->graphicsView();
	m_actor->GetProperty()->SetLineWidth(m_setting.arrowsSetting.lineWidth * v->devicePixelRatioF());

	actorCollection()->AddItem(m_actor);
	actor2DCollection()->AddItem(m_legendActor);
	updateVisibilityWithoutRendering();

	m_setting.arrowsSetting.legend.imageSetting.apply(v);

	topDataItem()->updateColorMapLegendsVisibility();
}

void Post2dWindowParticlesBaseVectorGroupDataItem::updateVisibility(bool visible)
{
	GraphicsWindowDataItem::updateVisibility(visible);

	auto v = isAncientChecked() && isChecked();
	m_arrowsToolBarWidget->setEnabled(v);
}

ColorMapSettingContainerI* Post2dWindowParticlesBaseVectorGroupDataItem::activeColorMapSetting() const
{
	if (! isAncientChecked() || ! isChecked()) {return nullptr;}
	if (m_setting.arrowsSetting.target == "") {return nullptr;}
	if (m_setting.arrowsSetting.colorMode == ArrowsSettingContainer::ColorMode::Custom) {return nullptr;}

	return topDataItem()->scalarGroupDataItem()->colorMapSetting(iRIC::toStr(m_setting.arrowsSetting.colorTarget));
}

ColorMapSettingContainerI* Post2dWindowParticlesBaseVectorGroupDataItem::activeColorMapSettingWithVisibleLegend() const
{
	auto cm = activeColorMapSetting();
	if (cm == nullptr) {return nullptr;}
	if (! cm->legendSetting()->getVisible()) {return nullptr;}

	return cm;
}

Post2dWindowGridTypeDataItem* Post2dWindowParticlesBaseVectorGroupDataItem::gridTypeDataItem() const
{
	return vectorTopDataItem()->zoneDataItem()->gridTypeDataItem();
}

Post2dWindowParticlesBaseTopDataItem* Post2dWindowParticlesBaseVectorGroupDataItem::topDataItem() const
{
	return vectorTopDataItem()->topDataItem();
}

Post2dWindowZoneDataItem* Post2dWindowParticlesBaseVectorGroupDataItem::zoneDataItem() const
{
	return vectorTopDataItem()->zoneDataItem();
}

vtkPolyData* Post2dWindowParticlesBaseVectorGroupDataItem::particleData() const
{
	return vectorTopDataItem()->particleData();
}

Post2dWindowParticlesBaseVectorGroupTopDataItem* Post2dWindowParticlesBaseVectorGroupDataItem::vectorTopDataItem() const
{
	return dynamic_cast<Post2dWindowParticlesBaseVectorGroupTopDataItem*> (parent());
}
