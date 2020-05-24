#ifndef POINTCONTROLLER_IMPL_H
#define POINTCONTROLLER_IMPL_H

#include "../pointcontroller.h"

#include "../../vtktool/vtklineactor.h"

class PointController::Impl
{
public:
	Impl();

	vtkLineActor m_actor;
};

#endif // POINTCONTROLLER_IMPL_H
