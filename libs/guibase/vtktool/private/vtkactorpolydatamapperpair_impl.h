#ifndef VTKACTORPOLYDATAMAPPERPAIR_IMPL_H
#define VTKACTORPOLYDATAMAPPERPAIR_IMPL_H

#include "../vtkactorpolydatamapperpair.h"

class vtkActorPolyDataMapperPair::Impl
{
public:
	Impl();
	~Impl();

	vtkActor* m_actor;
	vtkPolyDataMapper* m_mapper;
};

#endif // VTKACTORPOLYDATAMAPPERPAIR_IMPL_H
