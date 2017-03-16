#include "project.h"
#include "riversurveydatacreator.h"
#include "../base/dataItem.h"
#include "../crosssection/crosssection.h"
#include "../riversurveydata/riversurveydata.h"

#include "private/project_impl.h"

#include <map>

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

const QPointF& Project::offset() const
{
	return impl->m_offset;
}

QPointF& Project::offset()
{
	return impl->m_offset;
}

bool Project::sortCrossSectionsIfPossible()
{
	auto& bl = baseLine();
	auto csVec = crossSections().crossSectionVector();

	std::map<double, CrossSection*> posMap;
	bool crosses;
	double x, y, pos;

	for (CrossSection* cs : csVec) {
		bl.getCrossingPoint(*cs, &crosses, &x, &y, &pos);
		if (! crosses) {return false;}
		posMap.insert(std::make_pair(pos, cs));
	}

	auto& childItems = crossSections().childItems();
	childItems.clear();
	for (auto pair : posMap) {
		childItems.push_back(pair.second);
	}
	return true;
}

bool Project::checkIfReadyToOpenVerticalCrossSectionWindow(QWidget* w) const
{
	return true;
}

bool Project::checkIfReadyToCreateRiverSurveyData(QWidget* w) const
{
	return RiverSurveyDataCreator::checkIfReadyToCreate(*this, w);
}

void Project::importElevationPoints(QWidget* w)
{
	elevationPoints().importData(w);
}

void Project::importWaterSurfaceElevationPoints(QWidget* w)
{
	waterSurfaceElevationPoints().importData(w);
}

void Project::importBaseLine(QWidget* w)
{}

void Project::importCrossSections(QWidget* w)
{}

void Project::exportElevationPoints(QWidget* w)
{}

void Project::exportWaterSurfaceElevationPoints(QWidget *w)
{}

void Project::exportBaseLine(QWidget* w)
{}

void Project::exportCrossSections(QWidget* w)
{}

void Project::exportRiverSurveyData(QWidget* w)
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
