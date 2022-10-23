#include "post2dwindowgeodatatopdataitem.h"
#include "post2dwindowgridtypedataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/pre/base/preprocessordatamodelinterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

namespace {

PostZoneDataContainer* getContainerWithZoneType(const QList<PostZoneDataContainer*>& conts, SolverDefinitionGridType* gt)
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

void Post2dWindowGridTypeDataItem::setupZoneDataItems()
{
	// first, clear the current zonedata.
	for (const auto& z : m_zoneDatas) {
		delete z;
	}
	m_zoneDatas.clear();

	const auto& zones = postSolutionInfo()->zoneContainers2D();
	int num = 0;
	int zoneNum = 0;
	for (auto cont : zones) {
		if (cont->data() == nullptr) {continue;}
		if (cont->gridType() != m_gridType) {continue;}

		auto zdata = new Post2dWindowZoneDataItem(cont->zoneName(), num++, this);
		m_zoneDatas.push_back(zdata);
		m_zoneDataNameMap.insert({cont->zoneName(), zdata});
		m_childItems.push_back(zdata);
		++zoneNum;
	}

	PostZoneDataContainer* zCont = getContainerWithZoneType(zones, m_gridType);

	if (zCont != nullptr) {
		updateNodeValueRanges();
		updateCellValueRanges();
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

void Post2dWindowGridTypeDataItem::updateNodeValueRanges()
{
	m_nodeValueRanges.clear();

	for (auto zItem : m_zoneDatas) {
		if (zItem->dataContainer() == nullptr) {continue;}

		merge(zItem->dataContainer()->data()->valueRangeSet().pointDataValueRanges(), &m_nodeValueRanges);

		auto edgeIData = zItem->dataContainer()->edgeIData();
		if (edgeIData != nullptr) {
			merge(edgeIData->valueRangeSet().pointDataValueRanges(), &m_nodeValueRanges);
		}
		auto edgeJData = zItem->dataContainer()->edgeJData();
		if (edgeJData != nullptr) {
			merge(edgeJData->valueRangeSet().pointDataValueRanges(), &m_nodeValueRanges);
		}
	}
}

void Post2dWindowGridTypeDataItem::updateCellValueRanges()
{
	m_cellValueRanges.clear();

	for (auto zItem : m_zoneDatas) {
		if (zItem->dataContainer() == nullptr) {continue;}

		merge(zItem->dataContainer()->data()->valueRangeSet().cellDataValueRanges(), &m_cellValueRanges);
	}
}

void Post2dWindowGridTypeDataItem::updateParticleValueRanges()
{
	m_particleValueRanges.clear();

	for (auto zItem : m_zoneDatas) {
		if (zItem->dataContainer() == nullptr) {continue;}
		auto cont = zItem->dataContainer();

		auto pData = cont->particleData();
		if (pData != nullptr) {
			merge(pData->valueRangeSet().pointDataValueRanges(), &m_particleValueRanges);
		}
		for (const auto& pair : cont->particleGroupMap()) {
			merge(pair.second->valueRangeSet().pointDataValueRanges(), &m_particleValueRanges);
		}
	}
}

void Post2dWindowGridTypeDataItem::updatePolyDataValueRanges()
{
	m_polyDataValueRanges.clear();

	for (auto zItem : m_zoneDatas) {
		if (zItem->dataContainer() == nullptr) {continue;}
		auto cont = zItem->dataContainer();

		for (const auto& pair : cont->polyDataMap()) {
			merge(pair.second->valueRangeSet().cellDataValueRanges(), &m_polyDataValueRanges);
		}
	}
}

void Post2dWindowGridTypeDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
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
			Post2dWindowZoneDataItem* zdi = zoneData(zoneName);
			if (zdi != nullptr) {
				zdi->loadFromProjectMainFile(zoneNode);
			}
		}
	}
}

void Post2dWindowGridTypeDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("name", m_gridType->name().c_str());

	if (m_geoDataItem != nullptr) {
		writer.writeStartElement("GeoData");
		m_geoDataItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}

	writer.writeStartElement("Zones");
	for (auto zit = m_zoneDatas.begin(); zit != m_zoneDatas.end(); ++zit) {
		Post2dWindowZoneDataItem* zitem = *zit;
		writer.writeStartElement("Zone");
		zitem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	writer.writeEndElement();
}
