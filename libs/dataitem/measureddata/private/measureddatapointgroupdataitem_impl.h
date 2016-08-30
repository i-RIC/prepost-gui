#ifndef MEASUREDDATAPOINTGROUPDATAITEM_IMPL_H
#define MEASUREDDATAPOINTGROUPDATAITEM_IMPL_H

#include "../measureddatapointgroupdataitem.h"
#include "../measureddatapointsetting.h"

#include <unordered_map>

class vtkActor;
class vtkContourFilter;
class vtkLODActor;
class vtkPolyData;
class vtkPolyDataMapper;
class vtkScalarBarWidget;

class MeasuredDataPointGroupDataItem::Impl
{
public:
	Impl(MeasuredDataPointGroupDataItem* item);
	~Impl();

	void setupActors();
	void updateActorSettings();

	void setupPointSetting();
	void setupIsolineSetting();
	void setupColorContourSetting();
	void setupColorFringeSetting();

	void setupScalarBarSetting();

	void createValueClippedPolyData();

	MeasuredDataPointSetting m_setting;

	vtkLODActor* m_contourActor;
	vtkPolyDataMapper* m_contourMapper;

	vtkActor* m_isolineActor;
	vtkPolyDataMapper* m_isolineMapper;
	vtkContourFilter* m_isolineFilter;

	vtkLODActor* m_fringeActor;
	vtkPolyDataMapper* m_fringeMapper;

	vtkActor* m_pointsActor;
	vtkPolyDataMapper* m_pointsMapper;

	vtkActor* m_blackPointsActor;
	vtkPolyDataMapper* m_blackPointsMapper;

	// for scalar bar
	vtkScalarBarWidget* m_scalarBarWidget;
	std::unordered_map<std::string, LookupTableContainer*> m_lookupTables;
	std::unordered_map<std::string, QString> m_colorbarTitleMap;

	vtkPolyData* m_valueClippedPolyData;
	vtkPolyData* m_colorContourPolyData;

private:
	MeasuredDataPointGroupDataItem* m_item;
};

#endif // MEASUREDDATAPOINTGROUPDATAITEM_IMPL_H
