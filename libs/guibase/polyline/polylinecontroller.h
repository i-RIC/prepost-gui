#ifndef POLYLINECONTROLLER_H
#define POLYLINECONTROLLER_H

#include "../guibase_global.h"

#include <vector>

class QPointF;

class vtkActor;

class GUIBASEDLL_EXPORT PolyLineController
{
public:
	PolyLineController();
	~PolyLineController();

	std::vector<QPointF> polyLine() const;
	void setPolyLine(const std::vector<QPointF>& polyLine);

	bool isFocused() const;
	void setFocused(bool focused);

	bool isVertexSelectable(const QPointF& pos, double limitDistance, int* vid);
	bool isEdgeSelectable(const QPointF& pos, double limitDistance, int* edgeId);

	vtkActor* pointsActor() const;
	vtkActor* linesActor() const;

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/polylinecontroller_impl.h"
#endif // _DEBUG

#endif // POLYLINECONTROLLER_H
