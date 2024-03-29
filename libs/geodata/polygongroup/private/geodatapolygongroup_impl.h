#ifndef GEODATAPOLYGONGROUP_IMPL_H
#define GEODATAPOLYGONGROUP_IMPL_H

#include "../geodatapolygongroup.h"
#include "geodatapolygongroup_displaysetting.h"

class GeoDataPolygon;

class vtkActor;
class vtkPoints;
class vtkPolyData;

class GeoDataPolygonGroup::Impl
{
public:
	Impl(GeoDataPolygonGroup* group);
	~Impl();

	void setupDummyPolygonForMenu();

	vtkPoints* m_points;

	vtkPolyData* m_edgesPolyData;
	vtkPolyData* m_paintPolyData;

	vtkActor* m_edgesActor;
	vtkActor* m_paintActor;

	vtkPolyData* m_selectedPolygonsEdgesPolyData;
	vtkPolyData* m_selectedPolygonsPointsPolyData;

	vtkActor* m_selectedPolygonsEdgesActor;
	vtkActor* m_selectedPolygonsPointsActor;

	GeoDataPolygon* m_dummyPolygonForMenu;

	GeoDataPolygonGroup* m_group;

	DisplaySetting m_displaySetting;
};

#endif // GEODATAPOLYGONGROUP_IMPL_H

