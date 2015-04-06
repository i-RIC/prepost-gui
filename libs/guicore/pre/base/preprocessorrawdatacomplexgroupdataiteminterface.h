#ifndef PREPROCESSORRAWDATACOMPLEXGROUPDATAITEMINTERFACE_H
#define PREPROCESSORRAWDATACOMPLEXGROUPDATAITEMINTERFACE_H

#include <QList>

class Grid;
class ProjectData;
class RawDataBackgroundComplex;
class SolverDefinitionGridRelatedCondition;
class GridComplexConditionWidget;

class PreProcessorRawDataComplexGroupDataItemInterface {
public:
	virtual SolverDefinitionGridRelatedCondition* condition() = 0;
	virtual ProjectData* projectData() = 0;

protected:
	QList<GridComplexConditionWidget*> m_widgets;

public:
	friend class RawDataBackgroundComplex;
};


#endif // PREPROCESSORRAWDATACOMPLEXGROUPDATAITEMINTERFACE_H
