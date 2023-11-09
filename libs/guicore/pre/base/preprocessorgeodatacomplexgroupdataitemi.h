#ifndef PREPROCESSORGEODATACOMPLEXGROUPDATAITEMI_H
#define PREPROCESSORGEODATACOMPLEXGROUPDATAITEMI_H

#include <vector>

class ProjectData;
class GeoDataBackgroundComplex;
class SolverDefinitionGridAttribute;
class GridComplexConditionGroup;

class PreProcessorGeoDataComplexGroupDataItemI
{
public:
	virtual ~PreProcessorGeoDataComplexGroupDataItemI() {}

	virtual SolverDefinitionGridAttribute* condition() = 0;
	virtual ProjectData* projectData() const = 0;

	virtual void setupGroups(int count) = 0;
	virtual std::vector<GridComplexConditionGroup*> groups() const = 0;
};

#endif // PREPROCESSORGEODATACOMPLEXGROUPDATAITEMI_H
