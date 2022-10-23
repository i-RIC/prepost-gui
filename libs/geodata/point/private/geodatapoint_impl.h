#ifndef GEODATAPOINT_IMPL_H
#define GEODATAPOINT_IMPL_H

#include "../geodatapoint.h"

#include <guibase/point/pointcontroller.h>

class vtkActor;
class vtkDoubleArray;

class GeoDataPoint::Impl
{
public:
	Impl(GeoDataPoint* parent);
	~Impl();

	void setupScalarValues();

	GeoDataPoint* m_parent;
	vtkActor* m_actor;
	vtkDoubleArray* m_scalarValues;
	PointController m_pointController;

	MouseEventMode m_mouseEventMode;

	QPoint m_dragStartPoint;
	QPoint m_currentPoint;

	QMenu* m_rightClickingMenu;

	QAction* m_coordEditAction;
};

#endif // GEODATAPOINT_IMPL_H
