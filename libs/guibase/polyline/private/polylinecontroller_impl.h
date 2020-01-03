#ifndef POLYLINECONTROLLER_IMPL_H
#define POLYLINECONTROLLER_IMPL_H

#include "../polylinecontroller.h"

#include "../../vtktool/vtklineactor.h"

class vtkPolyData;

class PolyLineController::Impl
{
public:
	Impl();

	QPointF nearestPoint(int lineId, const QPointF& point);
	static QPointF nearestPoint(const QPointF& a, const QPointF& b, const QPointF& point);

	vtkLineActor m_actor;
};

#endif // POLYLINECONTROLLER_IMPL_H
