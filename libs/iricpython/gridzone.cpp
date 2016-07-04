#include "gridzone.h"

#include <pre/datamodel/preprocessorgridandgridcreatingconditiondataitem.h>
#include <pre/datamodel/preprocessorgridattributemappingsettingtopdataitem.h>

using namespace iRICPython;

GridZone::GridZone(PreProcessorGridAndGridCreatingConditionDataItem* item) :
	m_item {item}
{}

GridZone::~GridZone()
{}

void GridZone::mapAllGeoData()
{
	m_item->mappingSettingDataItem()->customMapping(true);
}

void GridZone::mapGeoData(const char* name)
{
	m_item->mappingSettingDataItem()->customMapping(name, true);
}
