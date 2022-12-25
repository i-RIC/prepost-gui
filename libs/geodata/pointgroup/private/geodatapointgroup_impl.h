#ifndef GEODATAPOINTGROUP_IMPL_H
#define GEODATAPOINTGROUP_IMPL_H

#include "../geodatapointgroup.h"
#include "../geodatapointgroupcolorsettingdialog.h"

class vtkActor;
class vtkPoints;
class vtkPolyData;

class GeoDataPointGroup::Impl
{
public:
	Impl(GeoDataPointGroup* group);
	~Impl();

	void setupDummyPointForMenu();

	vtkPoints* m_vtkPoints;

	vtkPolyData* m_pointsPolyData;
	vtkActor* m_pointsActor;

	vtkPolyData* m_selectedPointsPointsPolyData;
	vtkActor* m_selectedPointsPointsActor;

	GeoDataPoint* m_dummyPointForMenu;

	GeoDataPointGroupColorSettingDialog::Setting m_colorSetting;
};

#endif // GEODATAPOINTGROUP_IMPL_H
