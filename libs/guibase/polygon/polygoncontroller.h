#ifndef POLYGONCONTROLLER_H
#define POLYGONCONTROLLER_H

#include "../guibase_global.h"

class QPointF;
class QPolygonF;

class vtkActor;
class vtkPolyData;
class vtkPolygon;

class GUIBASEDLL_EXPORT PolygonController
{
public:
	PolygonController();
	~PolygonController();

	QPolygonF polygon() const;
	void setPolygon(const QPolygonF& polygon);

	vtkPolygon* getVtkPolygon() const;
	void applyVtkPolygonShape();

	bool isVertexSelectable(const QPointF& pos, double limitDistance, int* vid);
	bool isEdgeSelectable(const QPointF& pos, double limitDistance, int* edgeId);
	bool isAreaSelectable(const QPointF& pos);

	vtkPolyData* pointsPolyData() const;
	vtkPolyData* linesPolyData() const;
	vtkPolyData* polyData() const;

	vtkActor* pointsActor() const;
	vtkActor* linesActor() const;
	vtkActor* paintActor() const;

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/polygoncontroller_impl.h"
#endif // _DEBUG

#endif // POLYGONCONTROLLER_H
