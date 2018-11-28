#ifndef VTKPOLYGONSACTOR_IMPL_H
#define VTKPOLYGONSACTOR_IMPL_H

#include "../vtkpolygonsactor.h"
#include "../vtkpolydatalinesactor.h"
#include "../vtkpolydatapaintactor.h"
#include "../vtkpolydatapointsactor.h"

class vtkPolygonsActor::Impl
{
public:
	Impl();
	~Impl();

	vtkPolyDataPointsActor m_pointsActor;
	vtkPolyDataLinesActor m_linesActor;
	vtkPolyDataPaintActor m_paintActor;

	vtkPolyData* m_pointsPolyData;
	vtkPolyData* m_linesPolyData;
	vtkPolyData* m_polyData;
};

#endif // VTKPOLYGONSACTOR_IMPL_H
