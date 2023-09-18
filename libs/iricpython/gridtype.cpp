#include "geodatagroup.h"
#include "gridtype.h"
#include "gridzone.h"

#include <pre/datamodel/preprocessorgeodatagroupdataitem.h>
#include <pre/datamodel/preprocessorgridandgridcreatingconditiondataitem.h>
#include <pre/datamodel/preprocessorgridtypedataitem.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <guicore/pre/base/preprocessorgeodatatopdataitemi.h>

using namespace iRICPython;

GridType::GridType(const PreProcessorGridTypeDataItem* gtItem) :
	m_gtItem {gtItem},
	m_geoDataGroupsBuilt {false},
	m_geoDataGroups (),
	m_gridZonesBuilt {false},
	m_gridZones ()
{}

GridType::~GridType()
{
	for (auto group : m_geoDataGroups) {
		delete group;
	}
}

std::string GridType::name() const
{
	return m_gtItem->name();
}

bool GridType::isPrimary() const
{
	return m_gtItem->gridType()->isPrimary();
}

GeoDataGroup* GridType::geoDataGroup(const char* name) const
{
	for (auto g : geoDataGroups()) {
		if (g->name() == name) {
			return g;
		}
	}
	return nullptr;
}

std::vector<GeoDataGroup*> GridType::geoDataGroups() const
{
	if (! m_geoDataGroupsBuilt) {
		buildGeoDataGroups();
	}

	return m_geoDataGroups;
}

void GridType::mapAllGeoData()
{
	for (auto z : gridZones()) {
		z->mapAllGeoData();
	}
}

void GridType::mapGeoData(const char* name)
{
	for (auto z : gridZones()) {
		z->mapGeoData(name);
	}
}

std::vector<GridZone*> GridType::gridZones() const
{
	if (! m_gridZonesBuilt) {
		buildGridZones();
	}

	return m_gridZones;
}

void GridType::buildGeoDataGroups() const
{
	if (m_geoDataGroupsBuilt) {return;}

	auto groups = m_gtItem->geoDataTop()->groupDataItems();
	for (auto geog : groups) {
		auto g = new GeoDataGroup(dynamic_cast<PreProcessorGeoDataGroupDataItem*> (geog));
		m_geoDataGroups.push_back(g);
	}
	m_geoDataGroupsBuilt = true;
}

void GridType::buildGridZones() const
{
	if (m_gridZonesBuilt) {return;}

	for (auto c : m_gtItem->conditions()) {
		auto z = new GridZone(dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItem*> (c));
		m_gridZones.push_back(z);
	}
	m_gridZonesBuilt = true;
}

