#ifndef POLYLINECONTROLLER_IMPL_H
#define POLYLINECONTROLLER_IMPL_H

#include "../polylinecontroller.h"

#include "../../vtktool/vtklineactor.h"

class vtkPolyData;

class PolyLineController::Impl
{
public:
	Impl();

	vtkLineActor m_actor;
};

#endif // POLYLINECONTROLLER_IMPL_H
