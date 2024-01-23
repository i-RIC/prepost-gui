#include "../post3dwindowgraphicsview.h"
#include "post3dwindowgridtypedataitem.h"
#include "post3dwindownodevectorarrowdataitem.h"
#include "post3dwindownodevectorarrowgroupdataitem.h"
#include "post3dwindowzonedataitem.h"

#include <guicore/arrows/arrowssettingtoolbarwidget.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/scalarstocolors/colormapsettingcontainer.h>
#include <guicore/scalarstocolors/colormapsettingmodifycommand.h>
#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>
#include <misc/stringtool.h>
#include <misc/valuemodifycommandt.h>

#include <vtkActor.h>
#include <vtkTransformFilter.h>

Post3dWindowNodeVectorArrowDataItem::Setting::Setting() :
	CompositeContainer({&face, &filtering, &arrow})
{}

Post3dWindowNodeVectorArrowDataItem::Setting::Setting(const Setting& setting) :
	Setting {}
{
	copyValue(setting);
}

Post3dWindowNodeVectorArrowDataItem::Setting& Post3dWindowNodeVectorArrowDataItem::Setting::operator=(const Setting& c)
{
	copyValue(c);
	return *this;
}

XmlAttributeContainer& Post3dWindowNodeVectorArrowDataItem::Setting::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const Setting&> (c));
}

Post3dWindowNodeVectorArrowDataItem::Post3dWindowNodeVectorArrowDataItem(const QString& label, Post3dWindowDataItem* p) :
	Post3dWindowDataItem {"", QIcon(":/libs/guibase/images/iconPaper.svg"), p},
	m_actor {vtkActor::New()},
	m_setting {},
	m_zScale {1},
	m_arrowsToolBarWidget {new ArrowsSettingToolBarWidget(mainWindow())}
{
	setupStandardItem(Checked, NotReorderable, Deletable);
	m_standardItem->setText(label);

	m_arrowsToolBarWidget->hideLegendCheckbox();
	m_arrowsToolBarWidget->hide();
	m_arrowsToolBarWidget->setColorMapSettings(groupDataItem()->m_colorMapSettings);
	m_arrowsToolBarWidget->setSetting(&m_setting.arrow);

	connect(m_arrowsToolBarWidget, &ArrowsSettingToolBarWidget::updated, [=]() {
		auto com = new MergeSupportedListCommand(iRIC::generateCommandId("ArrowModify"), false);
		auto newSetting = m_arrowsToolBarWidget->modifiedSetting();
		com->addCommand(new ValueModifyCommmand<ArrowsSettingContainer>(iRIC::generateCommandId("ArrowsSetting"), false, newSetting, &m_setting.arrow));

		if (newSetting.colorMode == ArrowsSettingContainer::ColorMode::ByScalar) {
			auto cm = groupDataItem()->m_colorMapSettings.at(iRIC::toStr(newSetting.colorTarget));
			com->addCommand(new ColorMapSettingModifyCommand(m_arrowsToolBarWidget->modifiedColorMapSetting(), cm));
		}
		auto gItem = groupDataItem();
		gItem->pushUpdateActorSettingCommand(com, gItem);
	});

	setupActors();
}

Post3dWindowNodeVectorArrowDataItem::~Post3dWindowNodeVectorArrowDataItem()
{
	auto r = renderer();
	r->RemoveActor(m_actor);

	m_actor->Delete();
}

vtkStructuredGrid* Post3dWindowNodeVectorArrowDataItem::faceGrid() const
{
	auto data = groupDataItem()->data();
	if (data == nullptr) {return nullptr;}

	auto data2 = data->data();
	if (data2 == nullptr) {return nullptr;}

	return m_setting.face.extractFace(vtkStructuredGrid::SafeDownCast(data2->data()));
}

Post3dWindowNodeVectorArrowDataItem::Setting Post3dWindowNodeVectorArrowDataItem::setting() const
{
	auto s = m_setting;
	s.face.enabled.setValue(standardItem()->checkState() == Qt::Checked);

	return s;
}

void Post3dWindowNodeVectorArrowDataItem::setSetting(const Setting& setting)
{
	m_setting = setting;
}

void Post3dWindowNodeVectorArrowDataItem::update()
{
	updateActorSetting();
}

void Post3dWindowNodeVectorArrowDataItem::informSelection(VTKGraphicsView* v)
{
	groupDataItem()->informSelection(v);
}

void Post3dWindowNodeVectorArrowDataItem::informDeselection(VTKGraphicsView* v)
{
	groupDataItem()->informDeselection(v);
}

void Post3dWindowNodeVectorArrowDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	groupDataItem()->mouseMoveEvent(event, v);
}

void Post3dWindowNodeVectorArrowDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	groupDataItem()->mousePressEvent(event, v);
}

void Post3dWindowNodeVectorArrowDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	groupDataItem()->mouseReleaseEvent(event, v);
}

bool Post3dWindowNodeVectorArrowDataItem::addToolBarButtons(QToolBar* toolBar)
{
	groupDataItem()->addToolBarButtons(toolBar);

	toolBar->addSeparator();

	m_arrowsToolBarWidget->setParent(toolBar);
	m_arrowsToolBarWidget->show();

	toolBar->addWidget(m_arrowsToolBarWidget);

	return true;
}

void Post3dWindowNodeVectorArrowDataItem::handleStandardItemChange()
{
	GraphicsWindowDataItem::handleStandardItemChange();
	groupDataItem()->updateActorSetting();
}

void Post3dWindowNodeVectorArrowDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);

	updateActorSetting();
}

void Post3dWindowNodeVectorArrowDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);
}

void Post3dWindowNodeVectorArrowDataItem::innerUpdateZScale(double zscale)
{
	m_zScale = zscale;

	updateActorSetting();
}

void Post3dWindowNodeVectorArrowDataItem::innerUpdate2Ds()
{
	updateActorSetting();
}

void Post3dWindowNodeVectorArrowDataItem::setupActors()
{
	auto r = renderer();
	r->AddActor(m_actor);
}

void Post3dWindowNodeVectorArrowDataItem::updateActorSetting()
{
	m_actor->VisibilityOff();
	m_actorCollection->RemoveAllItems();

	auto data = groupDataItem()->data();
	if (data == nullptr) {return;}
	
	auto data2 = data->data();
	if (data2 == nullptr) {return;}

	auto grid = vtkStructuredGrid::SafeDownCast(data2->data());

	auto grid2 = m_setting.face.extractFace(grid);

	auto sampledData = m_setting.filtering.buildSampledData(grid2);
	grid2->Delete();

	auto filteredData = groupDataItem()->m_setting.buildFilteredData(sampledData);
	sampledData->Delete();

	const auto& groupSetting = groupDataItem()->m_setting;
	m_setting.arrow.target = groupSetting.target;
	m_setting.arrow.standardValue = groupSetting.standardValue;
	m_setting.arrow.legendLength = groupSetting.legendLength;
	m_setting.arrow.minimumValue = groupSetting.minimumValue;

	auto v = dataModel()->graphicsView();
	auto arrowsData = m_setting.arrow.buildArrowsPolygonData(filteredData, v, m_zScale);
	filteredData->Delete();

	if (m_setting.arrow.colorMode == ArrowsSettingContainer::ColorMode::Custom) {
		auto mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOff();
		auto gfilter = vtkSmartPointer<vtkGeometryFilter>::New();
		gfilter->SetInputData(arrowsData);
		mapper->SetInputConnection(gfilter->GetOutputPort());
		m_actor->SetMapper(mapper);
		mapper->Delete();
		m_actor->GetProperty()->SetColor(m_setting.arrow.customColor);
	} else if (m_setting.arrow.colorMode == ArrowsSettingContainer::ColorMode::ByScalar) {
		auto colorTarget = iRIC::toStr(m_setting.arrow.colorTarget);
		arrowsData->GetPointData()->SetActiveScalars(colorTarget.c_str());
		auto cs = groupDataItem()->colorMapSetting(colorTarget);
		if (cs == nullptr) {return;}
		auto mapper = cs->buildPointDataMapper(arrowsData);
		m_actor->SetMapper(mapper);
		mapper->Delete();
	}
	arrowsData->Delete();
	m_actor->GetProperty()->SetLineWidth(m_setting.arrow.lineWidth * v->devicePixelRatioF());

	actorCollection()->AddItem(m_actor);
	updateVisibilityWithoutRendering();
}

void Post3dWindowNodeVectorArrowDataItem::updateVisibility(bool visible)
{
	GraphicsWindowDataItem::updateVisibility(visible);

	groupDataItem()->updateLegendsVisibility();
}

Post3dWindowNodeVectorArrowGroupDataItem* Post3dWindowNodeVectorArrowDataItem::groupDataItem() const
{
	return dynamic_cast<Post3dWindowNodeVectorArrowGroupDataItem*> (parent());
}
