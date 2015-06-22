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
	virtual SolverDefinitionGridAttribute* condition() = 0;
	virtual ProjectData* projectData() = 0;

protected:
	QList<GridComplexConditionWidget*> m_widgets;

public:
	friend class GeoDataBackgroundComplex;
};


#endif // PREPROCESSORGEODATACOMPLEXGROUPDATAITEMINTERFACE_H
