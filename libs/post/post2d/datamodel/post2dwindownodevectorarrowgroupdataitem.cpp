#include "../post2dwindowgraphicsview.h"
#include "post2dwindowgridtypedataitem.h"
#include "post2dwindownodevectorarrowdataitem.h"
#include "post2dwindownodevectorarrowgroupdataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindownodevectorarrowgroupdataitem_updateactorsettingscommand.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guicore/named/namedgraphicswindowdataitemtool.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/scalarstocolors/colormapsettingcontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <guicore/solverdef/solverdefinitionoutput.h>
#include <misc/stringtool.h>
#include <misc/valuemodifycommandt.h>

#include <vtkActor.h>
#include <vtkActor2D.h>

Post2dWindowNodeVectorArrowGroupDataItem::Post2dWindowNodeVectorArrowGroupDataItem(Post2dWindowDataItem* p) :
	Post2dWindowDataItem {tr("Arrow"), QIcon(":/libs/guibase/images/iconFolder.svg"), p},
	m_actor {vtkActor::New()},
	m_arrowLegendActor {vtkActor2D::New()},
	m_colorLegendActor {vtkActor2D::New()}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	m_setting.legend.visibilityMode = ArrowsLegendSettingContainer::VisibilityMode::Always;

	PostZoneDataContainer* cont = zoneDataItem()->dataContainer();
	SolverDefinitionGridType* gt = cont->gridType();
	for (const auto& name : vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(cont->data()->data()->GetPointData())) {
		auto item = new Post2dWindowNodeVectorArrowDataItem(name, gt->solutionCaption(name), this);
		m_childItems.push_back(item);
	}

	for (const auto& pair : cont->data()->valueRangeSet().pointDataValueRanges()) {
		auto cs = new ColorMapSettingContainer();
		auto caption = gt->output(pair.first)->caption();
		cs->valueCaption = caption;
		cs->legend.title = caption;
		cs->setAutoValueRange(pair.second);
		m_colorMapSettings.insert({pair.first, cs});
	}

	setupActors();
}

Post2dWindowNodeVectorArrowGroupDataItem::~Post2dWindowNodeVectorArrowGroupDataItem()
{
	auto r = renderer();
	r->RemoveActor(m_actor);
	r->RemoveActor2D(m_arrowLegendActor);
	r->RemoveActor2D(m_colorLegendActor);

	m_actor->Delete();
	m_arrowLegendActor->Delete();
	m_colorLegendActor->Delete();

	for (const auto& pair : m_colorMapSettings) {
		delete pair.second;
	}
}

void Post2dWindowNodeVectorArrowGroupDataItem::handleNamedItemChange(NamedGraphicWindowDataItem* item)
{
	if (m_isCommandExecuting) {return;}

	auto newSetting = m_setting;
	if (item->standardItem()->checkState() == Qt::Checked) {
		newSetting.target = item->name().c_str();
		newSetting.legend.title = zoneDataItem()->gridTypeDataItem()->gridType()->output(item->name())->caption();
	} else {
		newSetting.target = "";
	}
	auto modifyCommand = new ValueModifyCommmand<ArrowsSettingContainer> ("Change target", newSetting, &m_setting);
	auto updateCommand = new UpdateActorSettingsCommand(false, modifyCommand, this);
	pushRenderCommand(updateCommand, this, true);
}

void Post2dWindowNodeVectorArrowGroupDataItem::setupActors()
{
	vtkRenderer* r = renderer();
	r->AddActor(m_actor);
	r->AddActor2D(m_arrowLegendActor);
	r->AddActor2D(m_colorLegendActor);

	m_actor->GetProperty()->SetLighting(false);

	m_setting.legend.imageSetting.setActor(m_arrowLegendActor);
	m_setting.legend.imageSetting.controller()->setItem(this);
}

void Post2dWindowNodeVectorArrowGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	auto childNodes = node.childNodes();
	for (int i = 0; i < childNodes.size(); ++i) {
		auto child = childNodes.at(i);
		if (child.nodeName() == "ColorMapSetting") {
			auto elem = child.toElement();
			auto name = iRIC::toStr(elem.attribute("name"));
			auto it = m_colorMapSettings.find(name);
			if (it == m_colorMapSettings.end()) {continue;}

			it->second->load(child);
		}
	}
}

void Post2dWindowNodeVectorArrowGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (const auto& pair : m_colorMapSettings) {
		writer.writeStartElement("ColorMapSetting");
		writer.writeAttribute("name", pair.first.c_str());
		pair.second->save(writer);
		writer.writeEndElement();
	}
}

void Post2dWindowNodeVectorArrowGroupDataItem::updateActorSettings()
{
	m_actor->VisibilityOff();
	m_arrowLegendActor->VisibilityOff();
	m_colorLegendActor->VisibilityOff();
	m_actorCollection->RemoveAllItems();
	m_actor2DCollection->RemoveAllItems();

	if (m_setting.target == "") {return;}

	auto data = buildFilteredData();
	if (data == nullptr) {return;}

	auto view = dataModel()->graphicsView();
	auto arrowsData = m_setting.buildArrowsPolygonData(data, view);
	data->Delete();
	auto v = dataModel()->graphicsView();

	if (m_setting.colorMode == ArrowsSettingContainer::ColorMode::Custom) {
		auto mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOff();
		auto gfilter = vtkSmartPointer<vtkGeometryFilter>::New();
		gfilter->SetInputData(arrowsData);
		mapper->SetInputConnection(gfilter->GetOutputPort());
		m_actor->SetMapper(mapper);
		mapper->Delete();

		m_actor->GetProperty()->SetColor(m_setting.customColor);
	} else if (m_setting.colorMode == ArrowsSettingContainer::ColorMode::ByScalar) {
		arrowsData->GetPointData()->SetActiveScalars(iRIC::toStr(m_setting.colorTarget).c_str());
		auto colorTarget = iRIC::toStr(m_setting.colorTarget);
		auto cs = colorMapSetting(colorTarget);
		auto mapper = cs->buildPointDataMapper(arrowsData);
		m_actor->SetMapper(mapper);
		mapper->Delete();
		cs->legend.imageSetting.setActor(m_colorLegendActor);
		cs->legend.imageSetting.controller()->setItem(this);
		cs->legend.imageSetting.controller()->handleSelection(v);
	}
	arrowsData->Delete();

	m_actor->GetProperty()->SetLineWidth(m_setting.lineWidth * v->devicePixelRatioF());

	m_actorCollection->AddItem(m_actor);
	updateVisibilityWithoutRendering();

	m_setting.legend.imageSetting.apply(v);
}

void Post2dWindowNodeVectorArrowGroupDataItem::updateCheckState()
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(m_setting.target, m_childItems, true);
}

Post2dWindowZoneDataItem* Post2dWindowNodeVectorArrowGroupDataItem::zoneDataItem() const
{
	return dynamic_cast<Post2dWindowZoneDataItem*> (parent());
}

ColorMapSettingContainer* Post2dWindowNodeVectorArrowGroupDataItem::colorMapSetting(const std::string& name) const
{
	return m_colorMapSettings.at(name);
}

void Post2dWindowNodeVectorArrowGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(1);
}

void Post2dWindowNodeVectorArrowGroupDataItem::assignActorZValues(const ZDepthRange& range)
{
	m_actor->SetPosition(0, 0, range.max());
}

void Post2dWindowNodeVectorArrowGroupDataItem::update()
{
	auto gtItem = zoneDataItem()->gridTypeDataItem();
	for (const auto& pair : m_colorMapSettings) {
		auto range = gtItem->nodeValueRange(pair.first);
		pair.second->setAutoValueRange(range);
	}

	updateActorSettings();
}

std::string Post2dWindowNodeVectorArrowGroupDataItem::target() const
{
	return iRIC::toStr(m_setting.target);
}

void Post2dWindowNodeVectorArrowGroupDataItem::setTarget(const std::string& target)
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(target, m_childItems);
	m_setting.target = target.c_str();

	updateActorSettings();
}

void Post2dWindowNodeVectorArrowGroupDataItem::innerUpdate2Ds()
{
	updateActorSettings();
}

void Post2dWindowNodeVectorArrowGroupDataItem::informSelection(VTKGraphicsView* v)
{
	zoneDataItem()->initNodeResultAttributeBrowser();

	m_setting.legend.imageSetting.controller()->handleSelection(v);

	if (m_setting.colorMode == ArrowsSettingContainer::ColorMode::Custom) {return;}
	auto s = colorMapSetting(iRIC::toStr(m_setting.colorTarget));
	if (s == nullptr) {return;}

	s->legend.imageSetting.controller()->handleSelection(v);
}

void Post2dWindowNodeVectorArrowGroupDataItem::informDeselection(VTKGraphicsView* v)
{
	zoneDataItem()->clearNodeResultAttributeBrowser();

	m_setting.legend.imageSetting.controller()->handleDeselection(v);

	if (m_setting.colorMode == ArrowsSettingContainer::ColorMode::Custom) {return;}
	auto s = colorMapSetting(iRIC::toStr(m_setting.colorTarget));
	if (s == nullptr) {return;}

	s->legend.imageSetting.controller()->handleDeselection(v);
}

void Post2dWindowNodeVectorArrowGroupDataItem::handleResize(VTKGraphicsView* v)
{
	m_setting.legend.imageSetting.controller()->handleResize(v);

	if (m_setting.colorMode == ArrowsSettingContainer::ColorMode::Custom) {return;}
	auto s = colorMapSetting(iRIC::toStr(m_setting.colorTarget));
	if (s == nullptr) {return;}

	s->legend.imageSetting.controller()->handleResize(v);
}

void Post2dWindowNodeVectorArrowGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	zoneDataItem()->updateNodeResultAttributeBrowser(event->pos(), v);
	std::vector<ImageSettingContainer::Controller*> controllers;

	m_setting.legend.imageSetting.controller()->handleMouseMoveEvent(event, v, true);
	controllers.push_back(m_setting.legend.imageSetting.controller());

	if (m_setting.colorMode == ArrowsSettingContainer::ColorMode::ByScalar) {
		auto s = colorMapSetting(iRIC::toStr(m_setting.colorTarget));
		if (s != nullptr) {
			s->legend.imageSetting.controller()->handleMouseMoveEvent(event, v, true);
			controllers.push_back(s->legend.imageSetting.controller());
		}
	}

	ImageSettingContainer::Controller::updateMouseCursor(v, controllers);
}

void Post2dWindowNodeVectorArrowGroupDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	std::vector<ImageSettingContainer::Controller*> controllers;

	m_setting.legend.imageSetting.controller()->handleMousePressEvent(event, v, true);
	controllers.push_back(m_setting.legend.imageSetting.controller());

	if (m_setting.colorMode == ArrowsSettingContainer::ColorMode::ByScalar) {
		auto s = colorMapSetting(iRIC::toStr(m_setting.colorTarget));
		if (s != nullptr) {
			s->legend.imageSetting.controller()->handleMousePressEvent(event, v, true);
			controllers.push_back(s->legend.imageSetting.controller());
		}
	}

	ImageSettingContainer::Controller::updateMouseCursor(v, controllers);
}

void Post2dWindowNodeVectorArrowGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	zoneDataItem()->fixNodeResultAttributeBrowser(event->pos(), v);
	std::vector<ImageSettingContainer::Controller*> controllers;

	m_setting.legend.imageSetting.controller()->handleMouseReleaseEvent(event, v, true);
	controllers.push_back(m_setting.legend.imageSetting.controller());

	if (m_setting.colorMode == ArrowsSettingContainer::ColorMode::ByScalar) {
		auto s = colorMapSetting(iRIC::toStr(m_setting.colorTarget));
		if (s != nullptr) {
			s->legend.imageSetting.controller()->handleMouseReleaseEvent(event, v, true);
			controllers.push_back(s->legend.imageSetting.controller());
		}
	}

	ImageSettingContainer::Controller::updateMouseCursor(v, controllers);
}

void Post2dWindowNodeVectorArrowGroupDataItem::addCustomMenuItems(QMenu* menu)
{
	QAction* abAction = zoneDataItem()->showAttributeBrowserActionForNodeResult();
	menu->addAction(abAction);
}
