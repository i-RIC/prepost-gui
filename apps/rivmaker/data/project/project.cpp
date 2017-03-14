#include "project.h"
#include "riversurveydatacreator.h"
#include "../base/dataItem.h"
#include "../riversurveydata/riversurveydata.h"

#include "private/project_impl.h"

Project::Impl::Impl(Project *project) :
	m_rootDataItem {project},
	m_elevationPoints {&m_rootDataItem},
	m_waterSurfaceElevationPoints {&m_rootDataItem},
	m_baseLine {&m_rootDataItem},
	m_crossSections {&m_rootDataItem},
	m_riverSurveyData {nullptr},
	m_riverSurveyDataDummy {&m_rootDataItem}
{}

Project::Impl::~Impl()
{
	delete m_riverSurveyData;
}

// public interfraces

Project::Project() :
	impl {new Impl {this}}
{}

Project::~Project()
{
	delete impl;
}

RootDataItem* Project::rootDataItem() const
{
	return &(impl->m_rootDataItem);
}

const ElevationPoints& Project::elevationPoints() const
{
	return impl->m_elevationPoints;
}

ElevationPoints& Project::elevationPoints()
{
	return impl->m_elevationPoints;
}

const WaterSurfaceElevationPoints& Project::waterSurfaceElevationPoints() const
{
	return impl->m_waterSurfaceElevationPoints;
}

WaterSurfaceElevationPoints& Project::waterSurfaceElevationPoints()
{
	return impl->m_waterSurfaceElevationPoints;
}

const BaseLine& Project::baseLine() const
{
	return impl->m_baseLine;
}

BaseLine& Project::baseLine()
{
	return impl->m_baseLine;
}

const CrossSections& Project::crossSections() const
{
	return impl->m_crossSections;
}

CrossSections& Project::crossSections()
{
	return impl->m_crossSections;
}

const std::vector<CrossSection*>& Project::crossSectionVector() const
{
	return impl->m_crossSectionVector;
}


std::vector<CrossSection*>& Project::crossSectionVector()
{
	return impl->m_crossSectionVector;
}

bool Project::hasRiverSurveyData() const
{
	return impl->m_riverSurveyData != nullptr;
}

RiverSurveyData* Project::riverSurveyData() const
{
	return impl->m_riverSurveyData;
}

const RiverSurveyDataDummy& Project::riverSurveyDataDummy() const
{
	return impl->m_riverSurveyDataDummy;
}

RiverSurveyDataDummy& Project::riverSurveyDataDummy()
{
	return impl->m_riverSurveyDataDummy;
}

void Project::importElevationPoints()
{}

bool Project::checkIfReadyToOpenVerticalCrossSectionWindow(QWidget* w) const
{
	return RiverSurveyDataCreator::checkIfReadyToOpenVerticalCrossSectionWindow(*this, w);
}

bool Project::checkIfReadyToCreateRiverSurveyData(QWidget* w) const
{
	return RiverSurveyDataCreator::checkIfReadyToCreate(*this, w);
}

void Project::importWaterSurfaceElevationPoints()
{}

void Project::importBaseLine()
{}

void Project::importCrossSections()
{}

void Project::exportElevationPoints()
{}

void Project::exportWaterSurfaceElevationPoints()
{}

void Project::exportBaseLine()
{}

void Project::exportCrossSections()
{}

void Project::exportRiverSurveyData()
{}

void Project::createRiverSurveyData()
{
	deleteRiverSurveyData();

	impl->m_riverSurveyData = RiverSurveyDataCreator::create(*this);
}

void Project::deleteRiverSurveyData()
{
	if (impl->m_riverSurveyData == nullptr) {return;}

	delete impl->m_riverSurveyData;
	impl->m_riverSurveyData = nullptr;
}
