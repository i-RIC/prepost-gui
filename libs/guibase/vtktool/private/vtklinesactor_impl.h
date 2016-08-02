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

	vtkPolyDataLinesActor m_linesActor;
	vtkPolyDataPointsActor m_pointsActor;

	vtkPolyData* m_polyData;
};

#endif // VTKLINESACTOR_IMPL_H
