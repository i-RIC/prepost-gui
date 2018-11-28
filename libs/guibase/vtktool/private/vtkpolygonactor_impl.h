#ifndef VTKPOLYGONACTOR_IMPL_H
#define VTKPOLYGONACTOR_IMPL_H

#include "../vtkpolygonactor.h"
#include "../vtkpolygonsactor.h"

class vtkPolygon;

class vtkPolygonActor::Impl
{
public:
	Impl();
	~Impl();

	vtkPolygonsActor m_actor;
	vtkPolygon* m_vtkPolygon;
};

#endif // VTKPOLYGONACTOR_IMPL_H
