#include "post2dbirdeyewindowgridtypedataitem.h"
#include "post2dbirdeyewindowzonedataitem.h"

#include <guibase/vtkpointsetextended/vtkpointsetextended.h>
#include <guibase/vtkpointsetextended/vtkpolydataextended2d.h>
#include <guibase/vtktool/vtkpointsetvaluerangeset.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

namespace {

v4PostZoneDataContainer* getContainerWithZoneType(const std::vector<v4PostZoneDataContainer*>& conts, SolverDefinitionGridType* gt)
{
	for (auto c : conts) {
		if (c->gridType() == gt) {
			return c;
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

Post2dBirdEyeWindowGridTypeDataItem::Post2dBirdEyeWindowGridTypeDataItem(SolverDefinitionGridType* type, GraphicsWindowDataItem* parent) :
	Post2dBirdEyeWindowDataItem {type->caption(), QIcon(":/libs/guibase/images/iconFolder.svg"), parent},
	m_gridType {type},
	m_isZoneDataItemsSetup {false}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);
	setSubPath(type->name().c_str());

	setupZoneDataItems();
}

Post2dBirdEyeWindowGridTypeDataItem::~Post2dBirdEyeWindowGridTypeDataItem()
{
	for (auto z_it = m_zoneDatas.begin(); z_it != m_zoneDatas.end(); ++z_it) {
		delete *z_it;
	}
}

const std::string& Post2dBirdEyeWindowGridTypeDataItem::name() const
{
	return m_gridType->name();
}

const std::vector<Post2dBirdEyeWindowZoneDataItem*>& Post2dBirdEyeWindowGridTypeDataItem::zoneDatas() const
{
	return m_zoneDatas;
}

Post2dBirdEyeWindowZoneDataItem* Post2dBirdEyeWindowGridTypeDataItem::zoneData(const std::string& name) const
{
	auto it = m_zoneDataNameMap.find(name);
	if (it == m_zoneDataNameMap.end()) {return nullptr;}

	return it->second;
}

SolverDefinitionGridType* Post2dBirdEyeWindowGridTypeDataItem::gridType() const
{
	return m_gridType;
}

const ValueRangeContainer& Post2dBirdEyeWindowGridTypeDataItem::nodeValueRange(const std::string& name) const
{
	const auto it = m_nodeValueRanges.find(name);
	if (it != m_nodeValueRanges.end()) {
		return it->second;
	}
	return m_dummyRange;
}

const std::unordered_map<std::string, ValueRangeContainer>& Post2dBirdEyeWindowGridTypeDataItem::nodeValueRanges() const
{
	return m_nodeValueRanges;
}

const ValueRangeContainer& Post2dBirdEyeWindowGridTypeDataItem::cellValueRange(const std::string& name) const
{
	const auto it = m_cellValueRanges.find(name);
	if (it != m_cellValueRanges.end()) {
		return it->second;
	}
	return m_dummyRange;
}

const std::unordered_map<std::string, ValueRangeContainer>& Post2dBirdEyeWindowGridTypeDataItem::cellValueRanges() const
{
	return m_cellValueRanges;
}

const ValueRangeContainer& Post2dBirdEyeWindowGridTypeDataItem::iEdgeValueRange(const std::string& name) const
{
	const auto it = m_iEdgeValueRanges.find(name);
	if (it != m_iEdgeValueRanges.end()) {
		return it->second;
	}
	return m_dummyRange;
}

const std::unordered_map<std::string, ValueRangeContainer>& Post2dBirdEyeWindowGridTypeDataItem::iEdgeValueRanges() const
{
	return m_iEdgeValueRanges;
}

const ValueRangeContainer& Post2dBirdEyeWindowGridTypeDataItem::jEdgeValueRange(const std::string& name) const
{
	const auto it = m_jEdgeValueRanges.find(name);
	if (it != m_jEdgeValueRanges.end()) {
		return it->second;
	}
	return m_dummyRange;
}

const std::unordered_map<std::string, ValueRangeContainer>& Post2dBirdEyeWindowGridTypeDataItem::JEdgeValueRanges() const
{
	return m_jEdgeValueRanges;
}

void Post2dBirdEyeWindowGridTypeDataItem::setupZoneDataItems()
{
	// first, clear the current zonedata.
	for (const auto& z : m_zoneDatas) {
		delete z;
	}
	m_zoneDatas.clear();

	const auto& zones = postSolutionInfo()->v4ZoneContainers2D();
	int num = 0;
	int zoneNum = 0;
	for (auto cont : zones) {
		if (cont->gridData() == nullptr) {continue;}
		if (cont->gridType() != m_gridType) {continue;}

		auto zdata = new Post2dBirdEyeWindowZoneDataItem(cont->zoneName(), num++, this);
		m_zoneDatas.push_back(zdata);
		m_zoneDataNameMap.insert({cont->zoneName(), zdata});
		m_childItems.push_back(zdata);
		++zoneNum;
	}

	v4PostZoneDataContainer* zCont = getContainerWithZoneType(zones, m_gridType);

	if (zCont != nullptr) {
		updateNodeValueRanges();
		updateCellValueRanges();
		updateIEdgeValueRanges();
		updateJEdgeValueRanges();
	}

	m_isZoneDataItemsSetup = (zoneNum != 0);
	if (m_isZoneDataItemsSetup) {update();}

	assignActorZValues(m_zDepthRange);
	updateItemMap();
}

void Post2dBirdEyeWindowGridTypeDataItem::update()
{
	if (! m_isZoneDataItemsSetup) {
		setupZoneDataItems();
	}

	// update value range.
	updateNodeValueRanges();
	updateCellValueRanges();
	updateIEdgeValueRanges();
	updateJEdgeValueRanges();

	// update child items.
	for (auto z : m_zoneDatas) {
		z->update();
	}
}

void Post2dBirdEyeWindowGridTypeDataItem::updateNodeValueRanges()
{
	m_nodeValueRanges.clear();

	for (auto zItem : m_zoneDatas) {
		auto cont = zItem->v4DataContainer();
		if (cont == nullptr || cont->gridData() == nullptr) {continue;}

		merge(cont->gridData()->grid()->vtkData()->valueRangeSet().pointDataValueRanges(), &m_nodeValueRanges);
	}
}

void Post2dBirdEyeWindowGridTypeDataItem::updateCellValueRanges()
{
	m_cellValueRanges.clear();

	for (auto zItem : m_zoneDatas) {
		auto cont = zItem->v4DataContainer();
		if (cont == nullptr || cont->gridData() == nullptr) {continue;}

		merge(cont->gridData()->grid()->vtkData()->valueRangeSet().cellDataValueRanges(), &m_cellValueRanges);
	}
}

void Post2dBirdEyeWindowGridTypeDataItem::updateIEdgeValueRanges()
{
	m_iEdgeValueRanges.clear();

	for (auto zItem : m_zoneDatas) {
		auto cont = zItem->v4DataContainer();
		if (cont == nullptr || cont->gridData() == nullptr) {continue;}

		auto sGrid = dynamic_cast<v4Structured2dGrid*> (cont->gridData()->grid());
		if (sGrid == nullptr) {continue;}

		merge(sGrid->vtkIEdgeData()->valueRangeSet().cellDataValueRanges(), &m_iEdgeValueRanges);
	}
}

void Post2dBirdEyeWindowGridTypeDataItem::updateJEdgeValueRanges()
{
	m_jEdgeValueRanges.clear();

	for (auto zItem : m_zoneDatas) {
		auto cont = zItem->v4DataContainer();
		if (cont == nullptr || cont->gridData() == nullptr) {continue;}

		auto sGrid = dynamic_cast<v4Structured2dGrid*> (cont->gridData()->grid());
		if (sGrid == nullptr) {continue;}

		merge(sGrid->vtkJEdgeData()->valueRangeSet().cellDataValueRanges(), &m_jEdgeValueRanges);
	}
}

void Post2dBirdEyeWindowGridTypeDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
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

void Post2dBirdEyeWindowGridTypeDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("name", m_gridType->name().c_str());

	writer.writeStartElement("Zones");
	for (auto zItem : m_zoneDatas) {
		writer.writeStartElement("Zone");
		zItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	writer.writeEndElement();
}
