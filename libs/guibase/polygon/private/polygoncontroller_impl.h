#ifndef POLYGONCONTROLLER_IMPL_H
#define POLYGONCONTROLLER_IMPL_H

#include "../polygoncontroller.h"

#include "../../vtktool/vtkpolygonactor.h"

class PolygonController::Impl
{
public:
	Impl();

	vtkPolygonActor m_actor;
};

#endif // POLYGONCONTROLLER_IMPL_H
