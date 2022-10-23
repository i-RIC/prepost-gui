#ifndef GEODATAPOINTMAP_POINTSMANAGER_H
#define GEODATAPOINTMAP_POINTSMANAGER_H

#include "../geodatapointmap.h"

class vtkLODActor;
class vtkPolyDataMapper;

class GeoDataPointmap::PointsManager
{
public:
	PointsManager(GeoDataPointmap* pointmap);
	~PointsManager();

	vtkPolyData* points() const;

	vtkLODActor* pointsActor() const;
	vtkLODActor* delaunayedTrianglesActor() const;

	vtkMaskPoints* maskPoints10k() const;
	vtkMaskPoints* maskPoints40k() const;

private:
	void setupSettings();

	vtkPolyData* m_points;

	vtkLODActor* m_pointsActor;
	vtkLODActor* m_delaunayedTrianglesActor;

	vtkMaskPoints* m_maskPoints10k;
	vtkMaskPoints* m_maskPoints40k;

	GeoDataPointmap* m_parent;
};

#endif // GEODATAPOINTMAP_POINTSMANAGER_H
