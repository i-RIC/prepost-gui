#include "../post3dwindow.h"
#include "post3dwindowgridtypedataitem.h"
#include "post3dwindowzonedataitem.h"

#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
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

Post3dWindowGridTypeDataItem::Post3dWindowGridTypeDataItem(SolverDefinitionGridType* type, GraphicsWindowDataItem* parent) :
	Post3dWindowDataItem {type->caption(), parent},
	m_gridType {type},
	m_isZoneDataItemsSetup {false}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	setSubPath(type->name().c_str());

	setupZoneDataItems();
}

Post3dWindowGridTypeDataItem::~Post3dWindowGridTypeDataItem()
{
	for (auto z_it = m_zoneDatas.begin(); z_it != m_zoneDatas.end(); ++z_it) {
		delete *z_it;
	}
}

const std::string& Post3dWindowGridTypeDataItem::name() const
{
	return m_gridType->name();
}

const std::vector<Post3dWindowZoneDataItem*>& Post3dWindowGridTypeDataItem::zoneDatas() const
{
	return m_zoneDatas;
}

Post3dWindowZoneDataItem* Post3dWindowGridTypeDataItem::zoneData(const std::string& name) const
{
	auto it = m_zoneDataNameMap.find(name);
	if (it == m_zoneDataNameMap.end()) {return nullptr;}

	return it->second;
}

SolverDefinitionGridType* Post3dWindowGridTypeDataItem::gridType() const
{
	return m_gridType;
}

const ValueRangeContainer& Post3dWindowGridTypeDataItem::nodeValueRange(const std::string& name) const
{
	const auto it = m_nodeValueRanges.find(name);
	if (it != m_nodeValueRanges.end()) {
		return it->second;
	}
	return m_dummyRange;
}

const std::unordered_map<std::string, ValueRangeContainer>& Post3dWindowGridTypeDataItem::nodeValueRanges() const
{
	return m_nodeValueRanges;
}

const ValueRangeContainer& Post3dWindowGridTypeDataItem::cellValueRange(const std::string& name) const
{
	const auto it = m_cellValueRanges.find(name);
	if (it != m_cellValueRanges.end()) {
		return it->second;
	}
	return m_dummyRange;
}

const std::unordered_map<std::string, ValueRangeContainer>& Post3dWindowGridTypeDataItem::cellValueRanges() const
{
	return m_cellValueRanges;
}

const ValueRangeContainer& Post3dWindowGridTypeDataItem::particleValueRange(const std::string& name) const
{
	const auto it = m_particleValueRanges.find(name);
	if (it != m_particleValueRanges.end()) {
		return it->second;
	}
	return m_dummyRange;
}

const std::unordered_map<std::string, ValueRangeContainer>& Post3dWindowGridTypeDataItem::particleValueRanges() const
{
	return m_particleValueRanges;
}

void Post3dWindowGridTypeDataItem::setupZoneDataItems()
{
	// first, clear the current zonedata.
	for (const auto& z : m_zoneDatas) {
		delete z;
	}
	m_zoneDatas.clear();

	const auto& zones = postSolutionInfo()->zoneContainers3D();
	int num = 0;
	int zoneNum = 0;
	for (auto cont : zones) {
		if (cont->data() == nullptr) {continue;}
		if (cont->gridType() != m_gridType) {continue;}

		auto zdata = new Post3dWindowZoneDataItem(cont->zoneName(), num++, this);
		m_zoneDatas.push_back(zdata);
		m_zoneDataNameMap.insert({cont->zoneName(), zdata});
		m_childItems.push_back(zdata);
		++ zoneNum;
	}

	PostZoneDataContainer* zCont = getContainerWithZoneType(zones, m_gridType);

	if (zCont != nullptr) {
		updateNodeValueRanges();
		updateCellValueRanges();
		updateParticleValueRanges();
	}

	m_isZoneDataItemsSetup = (zoneNum != 0);
	if (m_isZoneDataItemsSetup) {update();}

	updateItemMap();
}

void Post3dWindowGridTypeDataItem::update()
{
	if (! m_isZoneDataItemsSetup) {
		setupZoneDataItems();
	}

	// update value range.
	updateNodeValueRanges();
	updateCellValueRanges();
	updateParticleValueRanges();

	for (Post3dWindowZoneDataItem* item : m_zoneDatas) {
		item->update();
	}
}

void Post3dWindowGridTypeDataItem::updateNodeValueRanges()
{
	m_nodeValueRanges.clear();

	for (auto zItem : m_zoneDatas) {
		if (zItem->dataContainer() == nullptr) {continue;}

		merge(zItem->dataContainer()->data()->valueRangeSet().pointDataValueRanges(), &m_nodeValueRanges);
	}
}

void Post3dWindowGridTypeDataItem::updateCellValueRanges()
{
	m_cellValueRanges.clear();

	for (auto zItem : m_zoneDatas) {
		if (zItem->dataContainer() == nullptr) {continue;}

		merge(zItem->dataContainer()->data()->valueRangeSet().cellDataValueRanges(), &m_cellValueRanges);
	}
}

void Post3dWindowGridTypeDataItem::updateParticleValueRanges()
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

void Post3dWindowGridTypeDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomNode zonesNode = iRIC::getChildNode(node, "Zones");
	if (! zonesNode.isNull()) {
		QDomNodeList zones = zonesNode.childNodes();
		for (int i = 0; i < zones.length(); ++i) {
			QDomNode zoneNode = zones.at(i);
			std::string zoneName = iRIC::toStr(zoneNode.toElement().attribute("name"));
			Post3dWindowZoneDataItem* zdi = zoneData(zoneName);
			if (zdi != nullptr) {
				zdi->loadFromProjectMainFile(zoneNode);
			}
		}
	}
}

void Post3dWindowGridTypeDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("name", m_gridType->name().c_str());

	writer.writeStartElement("Zones");
	for (auto zit = m_zoneDatas.begin(); zit != m_zoneDatas.end(); ++zit) {
		Post3dWindowZoneDataItem* zitem = *zit;
		writer.writeStartElement("Zone");
		zitem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	writer.writeEndElement();
}
