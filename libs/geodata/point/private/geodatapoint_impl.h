#ifndef GEODATAPOINT_IMPL_H
#define GEODATAPOINT_IMPL_H

#include "../geodatapoint.h"
#include "geodatapoint_displaysetting.h"

#include <guibase/point/pointcontroller.h>

#include <QImage>

class vtkActor;
class vtkActor2D;
class vtkDoubleArray;

class GeoDataPoint::Impl
{
public:
	Impl(GeoDataPoint* parent);
	~Impl();

	static QPixmap shrinkPixmap(const QPixmap pixmap, int maxSize);
	void setupScalarValues();

	GeoDataPoint* m_parent;
	vtkActor* m_actor;
	vtkActor2D* m_imageActor;

	vtkDoubleArray* m_scalarValues;
	PointController m_pointController;

	MouseEventMode m_mouseEventMode;

	QPoint m_dragStartPoint;
	QPoint m_currentPoint;

	QMenu* m_rightClickingMenu;
	QImage m_shrinkedImage;

	QAction* m_coordEditAction;

	DisplaySetting m_displaySetting;
};

#endif // GEODATAPOINT_IMPL_H
