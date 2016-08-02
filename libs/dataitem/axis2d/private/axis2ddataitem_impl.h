#ifndef AXIS2DDATAITEM_IMPL_H
#define AXIS2DDATAITEM_IMPL_H

#include "../axis2ddataitem.h"

#include <vtkAxesActor.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkSmartPointer.h>

class Axis2dDataItem::Impl
{
public:
	Impl(Axis2dDataItem* parent);
	~Impl();

	void setupActors();

	vtkSmartPointer<vtkAxesActor> m_actor;
	vtkSmartPointer<vtkOrientationMarkerWidget> m_widget;

private:
	Axis2dDataItem* m_parent;
};

#endif // AXIS2DDATAITEM_IMPL_H
