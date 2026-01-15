#ifndef POST2DWINDOWBCDATAITEM_IMPL_H
#define POST2DWINDOWBCDATAITEM_IMPL_H

#include "../post2dwindowbcdataitem.h"
#include "post2dwindowbcdataitem_setting.h"

class vtkActor;
class vtkActor2D;
class vtkPolyData;
class vtkPolyDataMapper;
class vtkTextMapper;

class Post2dWindowBCDataItem::Impl
{
public:
	Impl(Post2dWindowBCDataItem* Item);
	~Impl();

	Setting m_setting;

	SolverDefinitionBoundaryCondition* m_condition;
	v4PostZoneDataBC* m_inputGridBC;

	vtkPolyData* m_data;
	vtkPolyDataMapper* m_mapper;
	vtkActor* m_actor;

	vtkActor2D* m_nameActor;
	vtkTextMapper* m_nameMapper;
};

#endif // POST2DWINDOWBCDATAITEM_IMPL_H
