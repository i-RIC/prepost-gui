#ifndef POINTCONTROLLER_H
#define POINTCONTROLLER_H

#include "../guibase_global.h"

class QPointF;

class vtkActor;
class vtkPolyData;

class GUIBASEDLL_EXPORT PointController
{
public:
	PointController();
	~PointController();

	QPointF point() const;
	void setPoint(const QPointF& point);
	void clear();
	bool pointIsSet() const;

	bool isSelectable(const QPointF& pos, double limitDistance);

	vtkPolyData* polyData();

	vtkActor* actor() const;

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/pointcontroller_impl.h"
#endif // _DEBUG

#endif // POINTCONTROLLER_H
