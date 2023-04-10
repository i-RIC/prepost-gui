#ifndef GEODATAPOLYLINEGROUP_IMPL_H
#define GEODATAPOLYLINEGROUP_IMPL_H

#include "../geodatapolylinegroup.h"
#include "geodatapolylinegroup_displaysetting.h"

class vtkActor;
class vtkPoints;
class vtkPolyData;

class GeoDataPolyLineGroup::Impl
{
public:
	Impl(GeoDataPolyLineGroup* group);
	~Impl();

	void setupDummyPolyLineForMenu();

	vtkPoints* m_points;

	vtkPolyData* m_edgesPolyData;
	vtkActor* m_edgesActor;

	vtkPolyData* m_selectedPolyLinesEdgesPolyData;
	vtkPolyData* m_selectedPolyLinesPointsPolyData;

	vtkActor* m_selectedPolyLinesEdgesActor;
	vtkActor* m_selectedPolyLinesPointsActor;

	GeoDataPolyLine* m_dummyPolyLineForMenu;

	DisplaySetting m_displaySetting;
};

#endif // GEODATAPOLYLINEGROUP_IMPL_H

