#include "../post3dwindowgraphicsview.h"
#include "post3dwindowgridtypedataitem.h"
#include "post3dwindowparticlesbasescalargroupdataitem.h"
#include "post3dwindowparticlesbasetopdataitem.h"
#include "post3dwindowparticlesbasevectorgroupdataitem.h"
#include "post3dwindowparticlesbasevectordataitem.h"
#include "post3dwindowzonedataitem.h"
#include "private/post3dwindowparticlesbasevectorgroupdataitem_propertydialog.h"

#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guicore/named/namedgraphicswindowdataitemtool.h>
#include <guicore/misc/targeted/targeteditemsettargetcommandtool.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/scalarstocolors/colormapsettingcontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <guibase/vtkdatasetattributestool.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>

#include <vtkTransform.h>
#include <vtkTransformFilter.h>

Post3dWindowParticlesBaseVectorGroupDataItem::Post3dWindowParticlesBaseVectorGroupDataItem(Post3dWindowDataItem* p) :
	Post3dWindowDataItem(tr("Vector"), QIcon(":/libs/guibase/images/iconFolder.svg"), p),
	m_actor {vtkActor::New()},
	m_legendActor {vtkActor2D::New()},
	m_transformFilter {vtkTransformFilter::New()},
	m_zScale {1}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	auto cont = zoneDataItem()->dataContainer();
	SolverDefinitionGridType* gt = cont->gridType();

	auto gtItem = gridTypeDataItem();
	for (auto name : vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(topDataItem()->particleData()->GetPointData())) {
		auto item = new Post3dWindowParticlesBaseVectorDataItem(name, gt->solutionCaption(name), this);
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

Post3dWindowParticlesBaseVectorGroupDataItem::~Post3dWindowParticlesBaseVectorGroupDataItem()
{
	auto r = renderer();
	r->RemoveActor(m_actor);
	r->RemoveActor2D(m_legendActor);

	m_actor->Delete();
	m_legendActor->Delete();
	m_transformFilter->Delete();
}

std::string Post3dWindowParticlesBaseVectorGroupDataItem::target() const
{
	return m_setting.arrowsSetting.target;
}

void Post3dWindowParticlesBaseVectorGroupDataItem::setTarget(const std::string& target)
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(target, m_childItems);
	m_setting.arrowsSetting.target = target.c_str();
	m_setting.arrowsSetting.legend.title = target.c_str();

	updateActorSettings();
}

void Post3dWindowParticlesBaseVectorGroupDataItem::update()
{
	updateActorSettings();
}

void Post3dWindowParticlesBaseVectorGroupDataItem::handleNamedItemChange(NamedGraphicWindowDataItem* item)
{
	if (m_isCommandExecuting) {return;}

	auto cmd = TargetedItemSetTargetCommandTool::buildFromNamedItem(item, this, tr("Arrow Physical Value Change"));
	pushRenderCommand(cmd, this, true);
}

void Post3dWindowParticlesBaseVectorGroupDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

QDialog* Post3dWindowParticlesBaseVectorGroupDataItem::propertyDialog(QWidget* p)
{
	return new PropertyDialog(this, p);
}

void Post3dWindowParticlesBaseVectorGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);
	updateCheckState();
	updateActorSettings();
}

void Post3dWindowParticlesBaseVectorGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);
}

void Post3dWindowParticlesBaseVectorGroupDataItem::innerUpdate2Ds()
{
	updateActorSettings();
}

void Post3dWindowParticlesBaseVectorGroupDataItem::innerUpdateZScale(double zscale)
{
	m_zScale = zscale;

	auto t = vtkSmartPointer<vtkTransform>::New();
	t->Scale(1, 1, zscale);
	m_transformFilter->SetTransform(t);

	updateActorSettings();
}

void Post3dWindowParticlesBaseVectorGroupDataItem::informSelection(VTKGraphicsView* v)
{
	auto& as = m_setting.arrowsSetting;
	as.legend.imageSetting.controller()->handleSelection(v);

	auto cs = activeSetting();
	if (cs != nullptr) {
		cs->legend.imageSetting.controller()->handleSelection(v);
	}
}

void Post3dWindowParticlesBaseVectorGroupDataItem::informDeselection(VTKGraphicsView* v)
{
	auto& as = m_setting.arrowsSetting;
	as.legend.imageSetting.controller()->handleDeselection(v);

	auto cs = activeSetting();
	if (cs != nullptr) {
		cs->legend.imageSetting.controller()->handleDeselection(v);
	}
}

void Post3dWindowParticlesBaseVectorGroupDataItem::handleResize(VTKGraphicsView* v)
{
	auto& as = m_setting.arrowsSetting;
	as.legend.imageSetting.controller()->handleResize(v);

	auto cs = activeSetting();
	if (cs != nullptr) {
		cs->legend.imageSetting.controller()->handleResize(v);
	}
}

void Post3dWindowParticlesBaseVectorGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
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

void Post3dWindowParticlesBaseVectorGroupDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
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

void Post3dWindowParticlesBaseVectorGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
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

void Post3dWindowParticlesBaseVectorGroupDataItem::setupActors()
{
	auto r = renderer();
	r->AddActor(m_actor);
	r->AddActor2D(m_legendActor);

	auto transform = vtkSmartPointer<vtkTransform>::New();
	m_transformFilter->SetTransform(transform);
}

void Post3dWindowParticlesBaseVectorGroupDataItem::updateCheckState()
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(m_setting.arrowsSetting.target, m_childItems, true);
}

void Post3dWindowParticlesBaseVectorGroupDataItem::updateActorSettings()
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

	m_transformFilter->SetInputData(sampledData);
	m_transformFilter->Update();
	sampledData->Delete();

	auto view = dataModel()->graphicsView();
	auto arrowsData = m_setting.arrowsSetting.buildArrowsPolygonData(m_transformFilter->GetOutput(), view, m_zScale);

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

ColorMapSettingContainer* Post3dWindowParticlesBaseVectorGroupDataItem::activeSetting() const
{
	if (m_setting.arrowsSetting.colorMode == ArrowsSettingContainer::ColorMode::Custom) {return nullptr;}

	return topDataItem()->scalarGroupDataItem()->colorMapSetting(iRIC::toStr(m_setting.arrowsSetting.colorTarget));
}

Post3dWindowGridTypeDataItem* Post3dWindowParticlesBaseVectorGroupDataItem::gridTypeDataItem() const
{
	return dynamic_cast<Post3dWindowGridTypeDataItem*> (zoneDataItem()->parent());
}

Post3dWindowParticlesBaseTopDataItem* Post3dWindowParticlesBaseVectorGroupDataItem::topDataItem() const
{
	return dynamic_cast<Post3dWindowParticlesBaseTopDataItem*> (parent());
}

Post3dWindowZoneDataItem* Post3dWindowParticlesBaseVectorGroupDataItem::zoneDataItem() const
{
	return topDataItem()->zoneDataItem();
}

vtkPolyData* Post3dWindowParticlesBaseVectorGroupDataItem::particleData() const
{
	return topDataItem()->particleData();
}
