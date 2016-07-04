#include "application.h"
#include "gridtype.h"
#include "pre.h"

#include <pre/preprocessorwindow.h>
#include <pre/preprocessordatamodel.h>
#include <pre/datamodel/preprocessorrootdataitem.h>

using namespace iRICPython;

Pre::Pre(const Application* app) :
	m_app {app},
	m_gridTypesBuilt {false},
	m_gridTypes()
{}

Pre::~Pre()
{}

GridType* Pre::gridType(const char* name) const
{
	for (auto gt : gridTypes()) {
		if (gt->name() == name) {
			return gt;
		}
	}
	return nullptr;
}

std::vector<GridType*> Pre::gridTypes() const
{
	if (! m_gridTypesBuilt) {
		buildGridTypes();
	}
	return m_gridTypes;
}

GeoDataGroup* Pre::geoDataGroup(const char* name) const
{
	auto gt = primaryGridType();
	if (gt == nullptr) {return nullptr;}

	return gt->geoDataGroup(name);
}

std::vector<GeoDataGroup*> Pre::geoDataGroups() const
{
	auto gt = primaryGridType();
	if (gt == nullptr) {
		std::vector<GeoDataGroup*> emptyVec;
		return emptyVec;
	}

	return gt->geoDataGroups();
}

void Pre::mapAllGeoData()
{
	auto gt = primaryGridType();
	if (gt == nullptr) {return;}

	gt->mapAllGeoData();
}

void Pre::mapGeoData(const char* name)
{
	auto gt = primaryGridType();
	if (gt == nullptr) {return;}

	gt->mapGeoData(name);
}

void Pre::clearGridTypes()
{
	for (auto gt : m_gridTypes) {
		delete gt;
	}
	m_gridTypes.clear();
	m_gridTypesBuilt = false;
}

GridType* Pre::primaryGridType() const
{
	for (auto gt : gridTypes()) {
		if (gt->isPrimary()) {
			return gt;
		}
	}
	return nullptr;
}

PreProcessorWindow* Pre::preWindow() const
{
	return m_app->preWindow();
}

void Pre::buildGridTypes() const
{
	if (m_gridTypesBuilt) {return;}

	auto model = dynamic_cast<PreProcessorDataModel*> (preWindow()->dataModel());
	auto gtItems = model->rootDataItem()->gridTypeDataItems();

	for (auto gtItem : gtItems) {
		auto gt = new GridType(gtItem);
		m_gridTypes.push_back(gt);
	}
	m_gridTypesBuilt = true;
}
