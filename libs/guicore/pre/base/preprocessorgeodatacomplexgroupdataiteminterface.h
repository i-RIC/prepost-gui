#ifndef PREPROCESSORGEODATACOMPLEXGROUPDATAITEMINTERFACE_H
#define PREPROCESSORGEODATACOMPLEXGROUPDATAITEMINTERFACE_H

#include <vector>

class Grid;
class ProjectData;
class GeoDataBackgroundComplex;
class SolverDefinitionGridAttribute;
class GridComplexConditionGroup;

class PreProcessorGeoDataComplexGroupDataItemInterface
{
public:
	virtual ~PreProcessorGeoDataComplexGroupDataItemInterface() {}

	virtual SolverDefinitionGridAttribute* condition() = 0;
	virtual ProjectData* projectData() const = 0;

	virtual void setupGroups(int count) = 0;
	virtual std::vector<GridComplexConditionGroup*> groups() const = 0;
};

#endif // PREPROCESSORGEODATACOMPLEXGROUPDATAITEMINTERFACE_H
