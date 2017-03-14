#ifndef PROJECT_IMPL_H
#define PROJECT_IMPL_H

#include "../project.h"
#include "../../base/rootdataitem.h"
#include "../../baseline/baseline.h"
#include "../../crosssections/crosssections.h"
#include "../../elevationpoints/elevationpoints.h"
#include "../../watersurfaceelevationpoints/watersurfaceelevationpoints.h"
#include "../../riversurveydatadummy/riversurveydatadummy.h"

class Project::Impl
{
public:
	Impl(Project* project);
	~Impl();

	RootDataItem m_rootDataItem;

	ElevationPoints m_elevationPoints;
	WaterSurfaceElevationPoints m_waterSurfaceElevationPoints;
	BaseLine m_baseLine;

	CrossSections m_crossSections;
	std::vector<CrossSection*> m_crossSectionVector;

	RiverSurveyData* m_riverSurveyData;
	RiverSurveyDataDummy m_riverSurveyDataDummy;
};

#endif // PROJECT_IMPL_H
