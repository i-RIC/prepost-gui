#include "measureddatafiledataitem.h"
#include "measureddatafiledataitem.h"
#include "measureddatapointgroupdataitem.h"
#include "measureddatavectordataitem.h"
#include "measureddatavectorgroupdataitem.h"
#include "measureddatavectorgrouptopdataitem.h"
#include "private/measureddatavectorgroupdataitem_impl.h"
#include "private/measureddatavectorgroupdataitem_settingeditwidget.h"

#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guicore/arrows/arrowssettingtoolbarwidget.h>
#include <guicore/datamodel/graphicswindowdataitemupdateactorsettingdialog.h>
#include <guicore/datamodel/graphicswindowdatamodel.h>
#include <guicore/datamodel/vtk2dgraphicsview.h>
#include <guicore/named/namedgraphicswindowdataitemtool.h>
#include <guicore/project/measured/measureddata.h>
#include <guicore/scalarstocolors/colormapsettingcontainer.h>
#include <guicore/scalarstocolors/colormapsettingmodifycommand.h>
#include <misc/iricundostack.h>
#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>
#include <misc/stringtool.h>
#include <misc/valuemodifycommandt.h>
#include <misc/xmlsupport.h>

#include <QIcon>
#include <QMainWindow>
#include <QStandardItem>
#include <QToolBar>

#include <vtkActor2D.h>
#include <vtkActor2DCollection.h>
#include <vtkGeometryFilter.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>

#include <cmath>
#include <vector>

MeasuredDataVectorGroupDataItem::Impl::Impl(MeasuredDataVectorGroupDataItem* item) :
	m_actor {vtkActor::New()},
	m_legendActor {vtkActor2D::New()},
	m_arrowsToolBarWidget {nullptr},
	m_item {item}
{}

MeasuredDataVectorGroupDataItem::Impl::~Impl()
{
	vtkRenderer* r = m_item->renderer();
	r->RemoveActor(m_actor);
	r->RemoveActor2D(m_legendActor);

	m_actor->Delete();
	m_legendActor->Delete();
}

void MeasuredDataVectorGroupDataItem::Impl::setupActors()
{
	vtkRenderer* r = m_item->renderer();
	r->AddActor(m_actor);
	r->AddActor2D(m_legendActor);

	m_actor->GetProperty()->SetLighting(false);
}

void MeasuredDataVectorGroupDataItem::Impl::updateCheckState()
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(m_setting.arrowsSetting.target, m_item->m_childItems, true);
}

// public interfaces

MeasuredDataVectorGroupDataItem::MeasuredDataVectorGroupDataItem(const std::string& target, GraphicsWindowDataItem* p) :
	GraphicsWindowDataItem {"", QIcon(":/libs/guibase/images/iconPaper.svg"), p},
	impl {new Impl(this)}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);
	impl->m_setting.arrowsSetting.target = target.c_str();
	impl->m_setting.arrowsSetting.legend.title = target.c_str();

	m_standardItem->setText(target.c_str());
	impl->setupActors();

	MeasuredData* md = topDataItem()->fileDataItem()->measuredData();

	impl->m_setting.arrowsSetting.legend.imageSetting.setActor(impl->m_legendActor);
	impl->m_setting.arrowsSetting.legend.imageSetting.controller()->setItem(this);

	if (md->scalarNames().size() > 0) {
		auto name = md->scalarNames().at(0);
		impl->m_setting.arrowsSetting.colorTarget = name.c_str();
	}

	impl->m_arrowsToolBarWidget = new ArrowsSettingToolBarWidget(mainWindow());
	impl->m_arrowsToolBarWidget->hide();
	impl->m_arrowsToolBarWidget->setColorMapSettings(topDataItem()->fileDataItem()->pointGroupDataItem()->colorMapSettings());
	impl->m_arrowsToolBarWidget->setSetting(&impl->m_setting.arrowsSetting);

	connect(impl->m_arrowsToolBarWidget, &ArrowsSettingToolBarWidget::updated, [=](){
		auto com = new MergeSupportedListCommand(iRIC::generateCommandId("ArrowModify"), false);
		auto newSetting = impl->m_arrowsToolBarWidget->modifiedSetting();
		com->addCommand(new ValueModifyCommmand<ArrowsSettingContainer>(iRIC::generateCommandId("ArrowsSetting"), false, newSetting, &impl->m_setting.arrowsSetting));

		if (newSetting.colorMode == ArrowsSettingContainer::ColorMode::ByScalar) {
			auto cm = topDataItem()->fileDataItem()->pointGroupDataItem()->colorMapSettings().at(iRIC::toStr(newSetting.colorTarget));
			com->addCommand(new ColorMapSettingModifyCommand(impl->m_arrowsToolBarWidget->modifiedColorMapSetting(), cm));
		}
		pushUpdateActorSettingCommand(com, this);
	});
}

MeasuredDataVectorGroupDataItem::~MeasuredDataVectorGroupDataItem()
{
	delete impl;
}

void MeasuredDataVectorGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_setting.load(node);
}

void MeasuredDataVectorGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	impl->m_setting.save(writer);
}

std::string MeasuredDataVectorGroupDataItem::target() const
{
	return impl->m_setting.arrowsSetting.target;
}

void MeasuredDataVectorGroupDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

void MeasuredDataVectorGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(2);
}

void MeasuredDataVectorGroupDataItem::assignActorZValues(const ZDepthRange& range)
{
	impl->m_actor->SetPosition(0, 0, range.max());
}

void MeasuredDataVectorGroupDataItem::update()
{
	updateActorSetting();
}

void MeasuredDataVectorGroupDataItem::innerUpdate2Ds()
{
	updateActorSetting();
}

ColorMapSettingContainerI* MeasuredDataVectorGroupDataItem::colorMapSetting(const std::string& target) const
{
	auto pItem = topDataItem()->fileDataItem()->pointGroupDataItem();
	return pItem->colorMapSetting(target);
}

ColorMapSettingContainerI* MeasuredDataVectorGroupDataItem::activeColorMapSetting() const
{
	if (! isAncientChecked() || ! isChecked()) {return nullptr;}
	if (impl->m_setting.arrowsSetting.target == "") {return nullptr;}
	if (impl->m_setting.arrowsSetting.colorMode == ArrowsSettingContainer::ColorMode::Custom) {return nullptr;}

	return colorMapSetting(iRIC::toStr(impl->m_setting.arrowsSetting.colorTarget));
}

ColorMapSettingContainerI* MeasuredDataVectorGroupDataItem::activeColorMapSettingWithVisibleLegend() const
{
	auto cm = activeColorMapSetting();
	if (cm == nullptr) {return nullptr;}
	if (! cm->legendSetting()->getVisible()) {return nullptr;}

	return cm;
}

QDialog* MeasuredDataVectorGroupDataItem::propertyDialog(QWidget* p)
{
	MeasuredData* md = topDataItem()->fileDataItem()->measuredData();
	if (md == nullptr || md->pointData() == nullptr) {
		return nullptr;
	}

	if (md->vectorNames().size() == 0) {
		return nullptr;
	}

	auto dialog = new GraphicsWindowDataItemUpdateActorSettingDialog(this, p);
	auto widget = new SettingEditWidget(this, dialog);
	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Arrows Display Setting (%1)").arg(target().c_str()));
	dialog->resize(900, 700);

	return dialog;
}

bool MeasuredDataVectorGroupDataItem::addToolBarButtons(QToolBar* toolBar)
{
	impl->m_arrowsToolBarWidget->setParent(toolBar);
	impl->m_arrowsToolBarWidget->show();

	toolBar->addWidget(impl->m_arrowsToolBarWidget);

	return true;
}

void MeasuredDataVectorGroupDataItem::doHandleResize(QResizeEvent* event, VTKGraphicsView* v)
{
	auto& as = impl->m_setting.arrowsSetting;
	as.legend.imageSetting.controller()->handleResize(event, v);

	auto cs = activeColorMapSetting();
	if (cs != nullptr) {
		cs->legendSetting()->imgSetting()->controller()->handleResize(event, v);
	}
}

void MeasuredDataVectorGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	std::vector<ImageSettingContainer::Controller*> controllers;

	auto& as = impl->m_setting.arrowsSetting;
	controllers.push_back(as.legend.imageSetting.controller());
	as.legend.imageSetting.controller()->handleMouseMoveEvent(event, v, true);

	auto cs = activeColorMapSetting();
	if (cs != nullptr) {
		controllers.push_back(cs->legendSetting()->imgSetting()->controller());
		cs->legendSetting()->imgSetting()->controller()->handleMouseMoveEvent(this, event, v, true);
	}

	ImageSettingContainer::Controller::updateMouseCursor(v, controllers);
}

void MeasuredDataVectorGroupDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	std::vector<ImageSettingContainer::Controller*> controllers;
	auto& as = impl->m_setting.arrowsSetting;
	controllers.push_back(as.legend.imageSetting.controller());
	as.legend.imageSetting.controller()->handleMousePressEvent(event, v, true);

	auto cs = activeColorMapSetting();
	if (cs != nullptr) {
		cs->legendSetting()->imgSetting()->controller()->handleMousePressEvent(this, event, v, true);
		controllers.push_back(cs->legendSetting()->imgSetting()->controller());
	}

	ImageSettingContainer::Controller::updateMouseCursor(v, controllers);
}

void MeasuredDataVectorGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	std::vector<ImageSettingContainer::Controller*> controllers;
	auto& as = impl->m_setting.arrowsSetting;
	as.legend.imageSetting.controller()->handleMouseReleaseEvent(event, v, true);
	controllers.push_back(as.legend.imageSetting.controller());

	auto cs = activeColorMapSetting();
	if (cs != nullptr) {
		cs->legendSetting()->imgSetting()->controller()->handleMouseReleaseEvent(this, event, v, true);
		controllers.push_back(cs->legendSetting()->imgSetting()->controller());
	}

	ImageSettingContainer::Controller::updateMouseCursor(v, controllers);
}

void MeasuredDataVectorGroupDataItem::doApplyOffset(double /*x*/, double /*y*/)
{
	updateActorSetting();
}

void MeasuredDataVectorGroupDataItem::handleStandardItemChange()
{
	GraphicsWindowDataItem::handleStandardItemChange();
	topDataItem()->fileDataItem()->updateColorMapLegendsVisibility();
}

void MeasuredDataVectorGroupDataItem::updateActorSetting()
{
	impl->m_actor->VisibilityOff();
	impl->m_legendActor->VisibilityOff();
	actorCollection()->RemoveAllItems();
	actor2DCollection()->RemoveAllItems();

	MeasuredData* md = topDataItem()->fileDataItem()->measuredData();
	if (md == nullptr || md->pointData() == nullptr) {return;}
	if (impl->m_setting.arrowsSetting.target == "") {return;}

	impl->m_setting.arrowsSetting.updateStandardValueIfNeeded(md->pointData()->GetPointData());
	auto filteredData = impl->m_setting.arrowsSetting.buildFilteredData(md->pointData());

	auto sampledData = impl->m_setting.filteringSetting.buildSampledData(filteredData);
	filteredData->Delete();

	auto view = dataModel()->graphicsView();
	auto arrowsData = impl->m_setting.arrowsSetting.buildArrowsPolygonData(sampledData, view);
	sampledData->Delete();

	auto cs = activeColorMapSetting();
	if (impl->m_setting.arrowsSetting.colorMode == ArrowsSettingContainer::ColorMode::Custom || cs == nullptr) {
		auto mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOff();
		auto gfilter = vtkSmartPointer<vtkGeometryFilter>::New();
		gfilter->SetInputData(arrowsData);
		mapper->SetInputConnection(gfilter->GetOutputPort());
		impl->m_actor->SetMapper(mapper);
		mapper->Delete();

		impl->m_actor->GetProperty()->SetColor(impl->m_setting.arrowsSetting.customColor);
	} else if (impl->m_setting.arrowsSetting.colorMode == ArrowsSettingContainer::ColorMode::ByScalar) {
		arrowsData->GetPointData()->SetActiveScalars(iRIC::toStr(impl->m_setting.arrowsSetting.colorTarget).c_str());
		auto mapper = cs->buildPointDataMapper(arrowsData);
		impl->m_actor->SetMapper(mapper);
		mapper->Delete();
	}
	arrowsData->Delete();

	auto v = dataModel()->graphicsView();
	impl->m_actor->GetProperty()->SetLineWidth(impl->m_setting.arrowsSetting.lineWidth * v->devicePixelRatioF());

	actorCollection()->AddItem(impl->m_actor);
	actor2DCollection()->AddItem(impl->m_legendActor);

	impl->m_setting.arrowsSetting.legend.imageSetting.apply(v);

	topDataItem()->fileDataItem()->updateColorMapLegendsVisibility();
}

void MeasuredDataVectorGroupDataItem::updateVisibility(bool visible)
{
	GraphicsWindowDataItem::updateVisibility(visible);

	auto v = isAncientChecked() && isChecked();
	impl->m_arrowsToolBarWidget->setEnabled(v);
}

MeasuredDataVectorGroupTopDataItem* MeasuredDataVectorGroupDataItem::topDataItem() const
{
	return dynamic_cast<MeasuredDataVectorGroupTopDataItem*> (parent());
}
