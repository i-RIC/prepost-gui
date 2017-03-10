#ifndef PROJECT_IMPL_H
#define PROJECT_IMPL_H

#include "../baseline.h"
#include "../crosssection.h"
#include "../elevationpoints.h"
#include "../project.h"
#include "../watersurfaceelevationpoints.h"

class Project::Impl
{
public:
	Impl();
	~Impl();

	ElevationPoints m_elevationPoints;
	WaterSurfaceElevationPoints m_waterSurfaceElevationPoints;
	BaseLine m_baseLine;

	RiverSurveyData* m_riverSurveyData;
};

#endif // PROJECT_IMPL_H
