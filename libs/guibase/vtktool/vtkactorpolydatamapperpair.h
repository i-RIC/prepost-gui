#ifndef VTKACTORPOLYDATAMAPPERPAIR_H
#define VTKACTORPOLYDATAMAPPERPAIR_H

#include "../guibase_global.h"

class vtkActor;
class vtkPolyDataMapper;

class GUIBASEDLL_EXPORT vtkActorPolyDataMapperPair
{
public:
	vtkActorPolyDataMapperPair();
	virtual ~vtkActorPolyDataMapperPair();

	vtkActor* actor() const;
	vtkPolyDataMapper* mapper() const;

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/vtkactorpolydatamapperpair_impl.h"
#endif // _DEBUG

#endif // VTKACTORPOLYDATAMAPPERPAIR_H
