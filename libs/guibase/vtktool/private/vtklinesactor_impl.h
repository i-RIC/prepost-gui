#ifndef VTKLINESACTOR_IMPL_H
#define VTKLINESACTOR_IMPL_H

#include "../vtklinesactor.h"
#include "../vtkpolydatalinesactor.h"
#include "../vtkpolydatapointsactor.h"

class vtkPolyData;

class vtkLinesActor::Impl
{
public:
	Impl();
	~Impl();

	vtkPolyDataPointsActor m_pointsActor;
	vtkPolyDataLinesActor m_linesActor;

	vtkPolyData* m_pointsPolyData;
	vtkPolyData* m_polyData;
};

#endif // VTKLINESACTOR_IMPL_H
