#ifndef AXIS3DDATAITEM_IMPL_H
#define AXIS3DDATAITEM_IMPL_H

#include "../axis3ddataitem.h"

#include <vtkAxesActor.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkSmartPointer.h>

class Axis3dDataItem::Impl
{
public:
	Impl(Axis3dDataItem* parent);
	~Impl();

	void setupActors();

	vtkSmartPointer<vtkAxesActor> m_actor;
	vtkSmartPointer<vtkOrientationMarkerWidget> m_widget;

private:
	Axis3dDataItem* m_parent;
};

#endif // AXIS3DDATAITEM_IMPL_H
