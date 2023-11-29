#include "post2dwindowgeodatagroupdataitem.h"
#include "post2dwindowgeodatatopdataitem.h"
#include "post2dwindowgridtypedataitem.h"
#include "post2dwindowinputgriddataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindowgridtypedataitem_applycolormapsettingandrendercommand.h"
#include "private/post2dwindowgridtypedataitem_applycolormapsettingdialog.h"
#include "private/post2dwindowgridtypedataitem_precolormapsettingupdatehandler.h"
#include "private/post2dwindowgridtypedataitem_toolbarwidgetcontroller.h"
#include "../post2dwindowgraphicsview.h"

#include <guibase/vtkpointsetextended/vtkpolydataextended2d.h>
#include <guicore/base/iricmainwindowi.h>
#include <guicore/grid/v4grid.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/image/imagesettingcontainer.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>
#include <guicore/pre/base/preprocessordatamodeli.h>
#include <guicore/pre/base/preprocessorgridtypedataitemi.h>
#include <guicore/pre/base/preprocessorwindowi.h>
#include <guicore/scalarstocolors/colormaplegendsettingcontaineri.h>
#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <guicore/scalarstocolors/colormapsettingtoolbarwidget.h>
#include <guicore/scalarstocolors/delegatedcolormapsettingcontainer.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridcomplexattribute.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>
#include <misc/valuechangert.h>

namespace {

v4PostZoneDataContainer* getContainerWithZoneType(const std::vector<v4PostZoneDataContainer*>& conts, SolverDefinitionGridType* gt)
{
	for (auto container : conts) {
		if (container->gridType() == gt) {
			return container;
		}
	}
	return nullptr;
}

ValueRangeContainer& getOrCreateValueRange(const std::string& name, std::unordered_map<std::string, ValueRangeContainer>* ranges, bool* created)
{
	*created = false;
	auto it = ranges->find(name);
	if (it != ranges->end()) {return it->second;}

	ValueRangeContainer range;
	auto pair = ranges->insert({name, range});
	*created = true;
	return pair.first->second;
}

void merge(const std::unordered_map<std::string, ValueRangeContainer>& src, std::unordered_map<std::string, ValueRangeContainer>* tgt)
{
	for (const auto& pair : src) {
		bool created;
		auto& r = getOrCreateValueRange(pair.first, tgt, &created);
		if (created) {
			r = pair.second;
		} else {
			r.merge(pair.second);
		}
	}
}

} // namespace

Post2dWindowGridTypeDataItem::Post2dWindowGridTypeDataItem(SolverDefinitionGridType* type, GraphicsWindowDataItem* parent) :
	Post2dWindowDataItem {type->caption(), QIcon(":/libs/guibase/images/iconFolder.svg"), parent},
	m_gridType {type},
	m_geoDataItem {nullptr},
	m_isZoneDataItemsSetup {false}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	setSubPath(type->name().c_str());

	auto gridTypeDataItem = dataModel()->iricMainWindow()->preProcessorWindow()->dataModel()->gridTypeDataItem(type->name());
	if (gridTypeDataItem != nullptr) {
		connect(gridTypeDataItem, &PreProcessorGridTypeDataItemI::colorMapSettingChanged, this, &Post2dWindowGridTypeDataItem::handlePreColorMapSettingUpdated);
		setupColorMapSettingContainers(gridTypeDataItem);
	}

	auto tItem = dataModel()->iricMainWindow()->preProcessorWindow()->dataModel()->geoDataTopDataItem(type->name());
	if (tItem != nullptr) {
		m_geoDataItem = new Post2dWindowGeoDataTopDataItem(tItem, this);
		m_childItems.push_back(m_geoDataItem);
	}

	setupZoneDataItems();
}

Post2dWindowGridTypeDataItem::~Post2dWindowGridTypeDataItem()
{
	for (auto z_it = m_zoneDatas.begin(); z_it != m_zoneDatas.end(); ++z_it) {
		delete *z_it;
	}

	for (auto& pair : m_colorMapSettingContainers) {
		delete pair.second;
	}

	auto r = renderer();
	for (auto pair : m_colorMapLegendActors) {
		r->RemoveActor2D(pair.second);
		pair.second->Delete();
	}
}

const std::string& Post2dWindowGridTypeDataItem::name() const
{
	return m_gridType->name();
}

const std::vector<Post2dWindowZoneDataItem*>& Post2dWindowGridTypeDataItem::zoneDatas() const
{
	return m_zoneDatas;
}

Post2dWindowZoneDataItem* Post2dWindowGridTypeDataItem::zoneData(const std::string& name) const
{
	auto it = m_zoneDataNameMap.find(name);
	if (it == m_zoneDataNameMap.end()) {return nullptr;}

	return it->second;
}

SolverDefinitionGridType* Post2dWindowGridTypeDataItem::gridType() const
{
	return m_gridType;
}

Post2dWindowGeoDataTopDataItem* Post2dWindowGridTypeDataItem::geoDataItem() const
{
	return m_geoDataItem;
}

ModifyCommandDialog* Post2dWindowGridTypeDataItem::createApplyColorMapSettingDialog(const std::string& name, QWidget* parent)
{
	return new ApplyColorMapSettingDialog(name, parent, this);
}

QUndoCommand* Post2dWindowGridTypeDataItem::createApplyColorMapSettingAndRenderCommand(const std::string& name, QUndoCommand* command, bool apply)
{
	return new ApplyColorMapSettingAndRenderCommand(command, name, apply, this);
}

const ValueRangeContainer& Post2dWindowGridTypeDataItem::nodeValueRange(const std::string& name) const
{
	const auto it = m_nodeValueRanges.find(name);
	if (it != m_nodeValueRanges.end()) {
		return it->second;
	}
	return m_dummyRange;
}

const std::unordered_map<std::string, ValueRangeContainer>& Post2dWindowGridTypeDataItem::nodeValueRanges() const
{
	return m_nodeValueRanges;
}

const ValueRangeContainer& Post2dWindowGridTypeDataItem::cellValueRange(const std::string& name) const
{
	const auto it = m_cellValueRanges.find(name);
	if (it != m_cellValueRanges.end()) {
		return it->second;
	}
	return m_dummyRange;
}

const std::unordered_map<std::string, ValueRangeContainer>& Post2dWindowGridTypeDataItem::cellValueRanges() const
{
	return m_cellValueRanges;
}

const ValueRangeContainer& Post2dWindowGridTypeDataItem::iEdgeValueRange(const std::string& name) const
{
	const auto it = m_iEdgeValueRanges.find(name);
	if (it != m_iEdgeValueRanges.end()) {
		return it->second;
	}
	return m_dummyRange;
}

const std::unordered_map<std::string, ValueRangeContainer>& Post2dWindowGridTypeDataItem::iEdgeValueRanges() const
{
	return m_iEdgeValueRanges;
}

const ValueRangeContainer& Post2dWindowGridTypeDataItem::jEdgeValueRange(const std::string& name) const
{
	const auto it = m_jEdgeValueRanges.find(name);
	if (it != m_jEdgeValueRanges.end()) {
		return it->second;
	}
	return m_dummyRange;
}

const std::unordered_map<std::string, ValueRangeContainer>& Post2dWindowGridTypeDataItem::jEdgeValueRanges() const
{
	return m_jEdgeValueRanges;
}

const ValueRangeContainer& Post2dWindowGridTypeDataItem::particleValueRange(const std::string& name) const
{
	const auto it = m_particleValueRanges.find(name);
	if (it != m_particleValueRanges.end()) {
		return it->second;
	}
	return m_dummyRange;
}

const std::unordered_map<std::string, ValueRangeContainer>& Post2dWindowGridTypeDataItem::particleValueRanges() const
{
	return m_particleValueRanges;
}

const ValueRangeContainer& Post2dWindowGridTypeDataItem::polyDataValueRange(const std::string& name) const
{
	const auto it = m_polyDataValueRanges.find(name);
	if (it != m_polyDataValueRanges.end()) {
		return it->second;
	}
	return m_dummyRange;
}

const std::unordered_map<std::string, ValueRangeContainer>& Post2dWindowGridTypeDataItem::polyDataValueRanges() const
{
	return m_polyDataValueRanges;
}

DelegatedColorMapSettingContainer* Post2dWindowGridTypeDataItem::colorMapSetting(const std::string& name) const
{
	auto it = m_colorMapSettingContainers.find(name);
	if (it == m_colorMapSettingContainers.end()) {return nullptr;}

	return it->second;
}

const std::unordered_map<std::string, DelegatedColorMapSettingContainer*>& Post2dWindowGridTypeDataItem::colorMapSettingContainers() const
{
	return m_colorMapSettingContainers;
}

void Post2dWindowGridTypeDataItem::updateColorBarVisibility(const std::string& attName)
{
	static bool updating = false;
	if (updating) {return;}

	ValueChangerT<bool> updatingChanger(&updating, true);

	auto cm = colorMapSetting(attName);
	if (cm == nullptr) {return;}
	auto actor = cm->activeImageSetting()->actor();
	m_actor2DCollection->RemoveItem(actor);
	actor->VisibilityOff();

	bool visible = false;
	if (m_geoDataItem == nullptr) {return;}

	auto gItem = m_geoDataItem->groupDataItem(attName);
	visible = visible || gItem->colorBarShouldBeVisible();

	for (auto zone : m_zoneDatas) {
		auto gItem = zone->inputGridDataItem();
		if (gItem == nullptr) {continue;}
		visible = visible || gItem->colorBarShouldBeVisible(attName);
	}
	visible = visible && cm->customSetting->legendSetting()->getVisible();
	if (visible) {
		auto v = dataModel()->graphicsView();
		cm->customSetting->legendSetting()->imgSetting()->apply(v);
		m_actor2DCollection->AddItem(actor);
	}

	updateVisibility();
}

void Post2dWindowGridTypeDataItem::setupZoneDataItems()
{
	// first, clear the current zonedata.
	for (const auto& z : m_zoneDatas) {
		delete z;
	}
	m_zoneDatas.clear();

	const auto& zones = postSolutionInfo()->v4ZoneContainers2D();
	int zoneNum = 0;
	for (auto cont : zones) {
		if (cont->gridType() != m_gridType) {continue;}

		auto zdata = new Post2dWindowZoneDataItem(cont->zoneName(), this);
		m_zoneDatas.push_back(zdata);
		m_zoneDataNameMap.insert({cont->zoneName(), zdata});
		m_childItems.push_back(zdata);
		++zoneNum;
	}

	auto zCont = getContainerWithZoneType(zones, m_gridType);

	if (zCont != nullptr) {
		updateNodeValueRanges();
		updateCellValueRanges();
		updateIEdgeValueRanges();
		updateJEdgeValueRanges();
		updateParticleValueRanges();
		updatePolyDataValueRanges();
	}

	assignActorZValues(m_zDepthRange);
	m_isZoneDataItemsSetup = (zoneNum != 0);
	updateItemMap();
}

void Post2dWindowGridTypeDataItem::update()
{
	if (! m_isZoneDataItemsSetup) {
		setupZoneDataItems();
	}

	// update value range.
	updateNodeValueRanges();
	updateCellValueRanges();
	updateParticleValueRanges();
	updatePolyDataValueRanges();

	// update child items.
	for (Post2dWindowZoneDataItem* item : m_zoneDatas) {
		item->update();
	}
}

void Post2dWindowGridTypeDataItem::doHandleResize(QResizeEvent* event, VTKGraphicsView* v)
{
	for (const auto& pair : m_colorMapSettingContainers) {
		pair.second->activeImageSetting()->controller()->handleResize(event, v);
	}
}

ColorMapSettingToolBarWidgetController* Post2dWindowGridTypeDataItem::createToolBarWidgetController(const std::string& name, QWidget* parent)
{
	auto att = m_gridType->gridAttribute(name);
	if (att == nullptr) {
		att = m_gridType->gridComplexAttribute(name);
		if (att == nullptr) {return nullptr;}
	}

	auto widget = att->createColorMapSettingToolbarWidget(parent);
	widget->hide();
	widget->setSetting(m_colorMapSettingContainers.at(name)->customSetting);
	return new ToolBarWidgetController(name, widget, this);
}

void Post2dWindowGridTypeDataItem::applyColorMapSetting(const std::string& name)
{
	auto i = m_geoDataItem->groupDataItem(name);
	i->applyColorMapSetting();

	updateColorBarVisibility(name);
}

void Post2dWindowGridTypeDataItem::handlePreColorMapSettingUpdated(const std::string& name)
{
	applyColorMapSetting(name);
	renderGraphicsView();
}

void Post2dWindowGridTypeDataItem::setupColorMapSettingContainers(PreProcessorGridTypeDataItemI* item)
{
	for (auto att : item->gridType()->gridAttributes()) {
		setupColorMapSettingContainer(att, item);
	}

	for (auto att : item->gridType()->gridComplexAttributes()) {
		setupColorMapSettingContainer(att, item);
	}
}

void Post2dWindowGridTypeDataItem::setupColorMapSettingContainer(SolverDefinitionGridAttribute* att, PreProcessorGridTypeDataItemI* item)
{
	auto r = renderer();
	auto v = dataModel()->graphicsView();

	auto c = new DelegatedColorMapSettingContainer();

	c->preSetting = att->createColorMapLegendSettingContainer();
	auto preColorMapSetting = item->colorMapSetting(att->name());
	c->preSetting->setSetting(preColorMapSetting);
	c->preSetting->copy(*c->preSetting->setting()->legendSetting());
	c->preSetting->setDelegateMode(true);
	auto handler = new PreColorMapSettingUpdateHandler(c, v, this);
	connect(preColorMapSetting, &ColorMapSettingContainerI::updated, handler, &PreColorMapSettingUpdateHandler::handle);

	c->customSetting = att->createColorMapSettingContainer();
	c->customSetting->copy(*c->preSetting->setting());
	m_colorMapSettingContainers.insert({att->name(), c});

	auto actor = vtkActor2D::New();
	r->AddActor2D(actor);
	c->preSetting->imgSetting()->setActor(actor);
	c->customSetting->legendSetting()->imgSetting()->setActor(actor);
	m_colorMapLegendActors.insert({att->name(), actor});
}

void Post2dWindowGridTypeDataItem::updateNodeValueRanges()
{
	m_nodeValueRanges.clear();

	for (auto zItem : m_zoneDatas) {
		auto cont = zItem->v4DataContainer();
		if (cont == nullptr) {continue;}

		merge(cont->gridData()->grid()->vtkData()->valueRangeSet().pointDataValueRanges(), &m_nodeValueRanges);
	}
}

void Post2dWindowGridTypeDataItem::updateCellValueRanges()
{
	m_cellValueRanges.clear();
	for (auto zItem : m_zoneDatas) {
		auto cont = zItem->v4DataContainer();
		if (cont == nullptr) {continue;}

		merge(cont->gridData()->grid()->vtkData()->valueRangeSet().cellDataValueRanges(), &m_cellValueRanges);
	}
}

void Post2dWindowGridTypeDataItem::updateIEdgeValueRanges()
{
	m_iEdgeValueRanges.clear();
	for (auto zItem : m_zoneDatas) {
		auto cont = zItem->v4DataContainer();
		if (cont == nullptr) {continue;}

		auto sGrid = dynamic_cast<v4Structured2dGrid*> (cont->gridData()->grid());
		if (sGrid == nullptr) {continue;}

		merge(sGrid->vtkIEdgeData()->valueRangeSet().cellDataValueRanges(), &m_iEdgeValueRanges);
	}
}

void Post2dWindowGridTypeDataItem::updateJEdgeValueRanges()
{
	m_jEdgeValueRanges.clear();
	for (auto zItem : m_zoneDatas) {
		auto cont = zItem->v4DataContainer();
		if (cont == nullptr) {continue;}

		auto sGrid = dynamic_cast<v4Structured2dGrid*> (cont->gridData()->grid());
		if (sGrid == nullptr) {continue;}

		merge(sGrid->vtkJEdgeData()->valueRangeSet().cellDataValueRanges(), &m_jEdgeValueRanges);
	}
}

void Post2dWindowGridTypeDataItem::updateParticleValueRanges()
{
	m_particleValueRanges.clear();
	for (auto zItem : m_zoneDatas) {
		auto cont = zItem->v4DataContainer();
		if (cont == nullptr) {continue;}

		auto pData = cont->particleData();
		if (pData != nullptr) {
			merge(pData->grid()->vtkData()->valueRangeSet().pointDataValueRanges(), &m_particleValueRanges);
		}
		for (const auto& pair : cont->particleGroupMap()) {
			merge(pair.second->grid()->vtkData()->valueRangeSet().pointDataValueRanges(), &m_particleValueRanges);
		}
	}
}

void Post2dWindowGridTypeDataItem::updatePolyDataValueRanges()
{
	m_polyDataValueRanges.clear();
	for (auto zItem : m_zoneDatas) {
		auto cont = zItem->v4DataContainer();
		if (cont == nullptr) {continue;}

		for (const auto& pair : cont->polyDataMap()) {
			merge(pair.second->grid()->vtkData()->valueRangeSet().cellDataValueRanges(), &m_polyDataValueRanges);
		}
	}
}

void Post2dWindowGridTypeDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	// load colormap data
	QDomNode cmNode = iRIC::getChildNode(node, "ColormapSettings");
	if (! cmNode.isNull()){
		for (int i = 0; i < cmNode.childNodes().count(); ++i) {
			QDomElement elem = cmNode.childNodes().at(i).toElement();
			std::string name = iRIC::toStr(elem.attribute("name"));
			auto it = m_colorMapSettingContainers.find(name);
			if (it != m_colorMapSettingContainers.end()) {
				it->second->load(elem);
			}
		}
	}

	QDomNode rNode = iRIC::getChildNode(node, "GeoData");
	if (! rNode.isNull() && m_geoDataItem != nullptr) {
		m_geoDataItem->loadFromProjectMainFile(rNode);
	}

	QDomNode zonesNode = iRIC::getChildNode(node, "Zones");
	if (! zonesNode.isNull()) {
		QDomNodeList zones = zonesNode.childNodes();
		for (int i = 0; i < zones.size(); ++i) {
			QDomNode zoneNode = zones.at(i);
			std::string zoneName = iRIC::toStr(zoneNode.toElement().attribute("name"));
			auto zdi = zoneData(zoneName);
			if (zdi != nullptr) {
				zdi->loadFromProjectMainFile(zoneNode);
			}
		}
	}
}

void Post2dWindowGridTypeDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("name", m_gridType->name().c_str());

	// write colormap data.
	writer.writeStartElement("ColormapSettings");
	for (const auto& pair : m_colorMapSettingContainers) {
		writer.writeStartElement("ColormapSetting");
		writer.writeAttribute("name", pair.first.c_str());
		pair.second->save(writer);
		writer.writeEndElement();
	}
	writer.writeEndElement();

	if (m_geoDataItem != nullptr) {
		writer.writeStartElement("GeoData");
		m_geoDataItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}

	writer.writeStartElement("Zones");
	for (auto zItem : m_zoneDatas) {
		writer.writeStartElement("Zone");
		zItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	writer.writeEndElement();
}
