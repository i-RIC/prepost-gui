#ifndef POST3DWINDOWNODESCALARGROUPDATAITEM_IMPL_H
#define POST3DWINDOWNODESCALARGROUPDATAITEM_IMPL_H

#include "../post3dwindownodescalargroupdataitem.h"
#include "post3dwindownodescalargroupdataitem_setting.h"

class vtkActor;
class vtkPolyDataMapper;

class Post3dWindowNodeScalarGroupDataItem::Impl
{
public:
	Impl();
	~Impl();

	Setting m_setting;

	vtkActor* m_isoSurfaceActor;
	vtkPolyDataMapper* m_isoSurfaceMapper;
};

#endif // POST3DWINDOWNODESCALARGROUPDATAITEM_IMPL_H
