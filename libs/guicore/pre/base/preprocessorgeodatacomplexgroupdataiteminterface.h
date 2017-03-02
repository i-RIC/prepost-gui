#ifndef PREPROCESSORGEODATACOMPLEXGROUPDATAITEMINTERFACE_H
#define PREPROCESSORGEODATACOMPLEXGROUPDATAITEMINTERFACE_H

#include <QList>

class Grid;
class ProjectData;
class GeoDataBackgroundComplex;
class SolverDefinitionGridAttribute;
class GridComplexConditionWidget;

class PreProcessorGeoDataComplexGroupDataItemInterface
{
public:
	virtual ~PreProcessorGeoDataComplexGroupDataItemInterface() {}

	virtual SolverDefinitionGridAttribute* condition() = 0;
	virtual ProjectData* projectData() const = 0;

	virtual void setupWidgets(int widgetCount) = 0;
	virtual QList<GridComplexConditionWidget*> widgets() const = 0;
};

#endif // PREPROCESSORGEODATACOMPLEXGROUPDATAITEMINTERFACE_H
