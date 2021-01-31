#ifndef GEODATAPOINTMAPREALBUILDER_H
#define GEODATAPOINTMAPREALBUILDER_H

#include "gd_pointmap_global.h"

class GeoData;
class GeoDataCreator;
class ProjectDataItem;
class SolverDefinitionGridAttribute;

class GD_POINTMAP_EXPORT GeoDataPointmapRealBuilder
{
public:
	GeoDataPointmapRealBuilder();
	~GeoDataPointmapRealBuilder();

	void begin();
	GeoData* end(ProjectDataItem* parent, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition);

	void addPoint(double x, double y, double z);

private:
	class Impl;
	Impl* impl;
};

#endif // GEODATAPOINTMAPREALBUILDER_H
