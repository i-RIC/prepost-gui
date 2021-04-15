#ifndef VTKARROWLEGENDACTORS_IMPL_H
#define VTKARROWLEGENDACTORS_IMPL_H

#include "../vtkarrowlegendactors.h"

class vtkPolyData;
class vtkPolyDataMapper2D;

class vtkArrowLegendActors::Impl
{
public:
	Impl();
	~Impl();

	vtkTextActor* m_textActor;
	vtkActor2D* m_arrowActor;

	vtkPolyDataMapper2D* m_mapper;
};

#endif // VTKARROWLEGENDACTORS_IMPL_H
