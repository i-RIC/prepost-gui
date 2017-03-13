#ifndef PROJECT_IMPL_H
#define PROJECT_IMPL_H

#include "../project.h"
#include "../../baseline/baseline.h"
#include "../../elevationpoints/elevationpoints.h"
#include "../../watersurfaceelevationpoints/watersurfaceelevationpoints.h"

class Project::Impl
{
public:
	Impl();
	~Impl();

	ElevationPoints m_elevationPoints;
	WaterSurfaceElevationPoints m_waterSurfaceElevationPoints;
	BaseLine m_baseLine;
	std::vector<CrossSection*> m_crossSections;

	RiverSurveyData* m_riverSurveyData;
};

#endif // PROJECT_IMPL_H
