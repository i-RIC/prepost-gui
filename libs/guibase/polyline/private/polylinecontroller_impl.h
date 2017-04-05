#ifndef POLYLINECONTROLLER_IMPL_H
#define POLYLINECONTROLLER_IMPL_H

#include "../polylinecontroller.h"

#include "../../vtktool/vtkpolydatalinesactor.h"
#include "../../vtktool/vtkpolydatapointsactor.h"

class vtkPolyData;

class PolyLineController::Impl
{
public:
	Impl();
	~Impl();

	bool m_isFocused;

	vtkPolyData* m_polyData;

	vtkPolyDataLinesActor m_linesActor;
	vtkPolyDataPointsActor m_pointsActor;
};

#endif // POLYLINECONTROLLER_IMPL_H
