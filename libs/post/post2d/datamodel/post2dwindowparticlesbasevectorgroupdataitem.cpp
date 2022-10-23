#include "../post2dwindowgraphicsview.h"
#include "post2dwindowgridtypedataitem.h"
#include "post2dwindowparticlesbasescalargroupdataitem.h"
#include "post2dwindowparticlesbasetopdataitem.h"
#include "post2dwindowparticlesbasevectorgroupdataitem.h"
#include "post2dwindowparticlesbasevectordataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindowparticlesbasevectorgroupdataitem_propertydialog.h"

#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guicore/named/namedgraphicswindowdataitemtool.h>
#include <guicore/misc/targeted/targeteditemsettargetcommandtool.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/scalarstocolors/colormapsettingcontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <guibase/vtkdatasetattributestool.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>

Post2dWindowParticlesBaseVectorGroupDataItem::Post2dWindowParticlesBaseVectorGroupDataItem(Post2dWindowDataItem* p) :
	Post2dWindowDataItem(tr("Vector"), QIcon(":/libs/guibase/images/iconFolder.svg"), p),
	m_actor {vtkActor::New()},
	m_legendActor {vtkActor2D::New()}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	auto cont = zoneDataItem()->dataContainer();
	SolverDefinitionGridType* gt = cont->gridType();

	for (auto name : vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(topDataItem()->particleData()->GetPointData())) {
		auto item = new Post2dWindowParticlesBaseVectorDataItem(name, gt->solutionCaption(name), this);
		m_childItems.push_back(item);
	}

	m_setting.arrowsSetting.legend.imageSetting.setActor(m_legendActor);
	m_setting.arrowsSetting.legend.imageSetting.controller()->setItem(this);
	m_setting.arrowsSetting.legend.visibilityMode = ArrowsLegendSettingContainer::VisibilityMode::Always;

	bool first = true;
	for (auto name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(topDataItem()->particleData()->GetPointData())) {
		if (first) {
			m_setting.arrowsSetting.colorTarget = name.c_str();
		}
		first = false;
	}

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

void Post2dWindowParticlesBaseVectorGroupDataItem::setTarget(const std::string& target)
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(target, m_childItems);
	m_setting.arrowsSetting.target = target.c_str();
	m_setting.arrowsSetting.legend.title = target.c_str();

	updateActorSettings();
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
	updateActorSettings();
}

void Post2dWindowParticlesBaseVectorGroupDataItem::handleNamedItemChange(NamedGraphicWindowDataItem* item)
{
	if (m_isCommandExecuting) {return;}

	auto cmd = TargetedItemSetTargetCommandTool::buildFromNamedItem(item, this, tr("Arrow Physical Value Change"));
	pushRenderCommand(cmd, this, true);
}

void Post2dWindowParticlesBaseVectorGroupDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

QDialog* Post2dWindowParticlesBaseVectorGroupDataItem::propertyDialog(QWidget* p)
{
	return new PropertyDialog(this, p);
}

void Post2dWindowParticlesBaseVectorGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);
	updateCheckState();
	updateActorSettings();
}

void Post2dWindowParticlesBaseVectorGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);
}

void Post2dWindowParticlesBaseVectorGroupDataItem::innerUpdate2Ds()
{
	updateActorSettings();
}

void Post2dWindowParticlesBaseVectorGroupDataItem::informSelection(VTKGraphicsView* v)
{
	auto& as = m_setting.arrowsSetting;
	as.legend.imageSetting.controller()->handleSelection(v);

	auto cs = activeSetting();
	if (cs != nullptr) {
		cs->legend.imageSetting.controller()->handleSelection(v);
	}

	zoneDataItem()->initParticleResultAttributeBrowser(particleData());
}

void Post2dWindowParticlesBaseVectorGroupDataItem::informDeselection(VTKGraphicsView* v)
{
	auto& as = m_setting.arrowsSetting;
	as.legend.imageSetting.controller()->handleDeselection(v);

	auto cs = activeSetting();
	if (cs != nullptr) {
		cs->legend.imageSetting.controller()->handleDeselection(v);
	}

	zoneDataItem()->clearParticleResultAttributeBrowser();
}

void Post2dWindowParticlesBaseVectorGroupDataItem::handleResize(VTKGraphicsView* v)
{
	auto& as = m_setting.arrowsSetting;
	as.legend.imageSetting.controller()->handleResize(v);

	auto cs = activeSetting();
	if (cs != nullptr) {
		cs->legend.imageSetting.controller()->handleResize(v);
	}
}

void Post2dWindowParticlesBaseVectorGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	zoneDataItem()->updateParticleResultAttributeBrowser(event->pos(), v);
	std::vector<ImageSettingContainer::Controller*> controllers;

	auto& as = m_setting.arrowsSetting;
	as.legend.imageSetting.controller()->handleMouseMoveEvent(event, v, true);
	controllers.push_back(as.legend.imageSetting.controller());

	auto cs = activeSetting();
	if (cs != nullptr) {
		cs->legend.imageSetting.controller()->handleMouseMoveEvent(event, v, true);
		controllers.push_back(cs->legend.imageSetting.controller());
	}

	ImageSettingContainer::Controller::updateMouseCursor(v, controllers);
}

void Post2dWindowParticlesBaseVectorGroupDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	std::vector<ImageSettingContainer::Controller*> controllers;

	auto& as = m_setting.arrowsSetting;
	as.legend.imageSetting.controller()->handleMousePressEvent(event, v, true);
	controllers.push_back(as.legend.imageSetting.controller());

	auto cs = activeSetting();
	if (cs != nullptr) {
		cs->legend.imageSetting.controller()->handleMousePressEvent(event, v, true);
		controllers.push_back(cs->legend.imageSetting.controller());
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

	auto cs = activeSetting();
	if (cs != nullptr) {
		cs->legend.imageSetting.controller()->handleMouseReleaseEvent(event, v, true);
		controllers.push_back(cs->legend.imageSetting.controller());
	}

	ImageSettingContainer::Controller::updateMouseCursor(v, controllers);
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

void Post2dWindowParticlesBaseVectorGroupDataItem::updateActorSettings()
{
	m_actor->VisibilityOff();
	m_legendActor->VisibilityOff();
	actorCollection()->RemoveAllItems();
	actor2DCollection()->RemoveAllItems();

	auto data = particleData();
	if (data == nullptr) {return;}
	if (m_setting.arrowsSetting.target == "") {return;}

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
		auto cs = activeSetting();
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

	m_setting.arrowsSetting.legend.imageSetting.controller()->handleSelection(v);

	auto& as = m_setting.arrowsSetting;
	if (as.colorMode == ArrowsSettingContainer::ColorMode::Custom) {return;}

	auto cs = activeSetting();
	if (cs == nullptr) {return;}

	cs->legend.imageSetting.apply(v);
}

ColorMapSettingContainer* Post2dWindowParticlesBaseVectorGroupDataItem::activeSetting() const
{
	if (m_setting.arrowsSetting.colorMode == ArrowsSettingContainer::ColorMode::Custom) {return nullptr;}

	return topDataItem()->scalarGroupDataItem()->colorMapSetting(iRIC::toStr(m_setting.arrowsSetting.colorTarget));
}

Post2dWindowGridTypeDataItem* Post2dWindowParticlesBaseVectorGroupDataItem::gridTypeDataItem() const
{
	return dynamic_cast<Post2dWindowGridTypeDataItem*> (zoneDataItem()->parent());
}

Post2dWindowParticlesBaseTopDataItem* Post2dWindowParticlesBaseVectorGroupDataItem::topDataItem() const
{
	return dynamic_cast<Post2dWindowParticlesBaseTopDataItem*> (parent());
}

Post2dWindowZoneDataItem* Post2dWindowParticlesBaseVectorGroupDataItem::zoneDataItem() const
{
	return topDataItem()->zoneDataItem();
}

vtkPolyData* Post2dWindowParticlesBaseVectorGroupDataItem::particleData() const
{
	return topDataItem()->particleData();
}
