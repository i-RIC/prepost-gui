#ifndef VTKPOLYGONACTOR_H
#define VTKPOLYGONACTOR_H

#include "../guibase_global.h"

class QPolygonF;

class vtkActor;
class vtkPolyData;
class vtkPolygon;

class GUIBASEDLL_EXPORT vtkPolygonActor
{
public:
	vtkPolygonActor();
	~vtkPolygonActor();

	vtkActor* pointsActor();
	vtkActor* linesActor();
	vtkActor* paintActor();

	vtkPolyData* pointsPolyData() const;
	vtkPolyData* linesPolyData() const;
	vtkPolyData* polygonPolyData() const;

	QPolygonF polygon() const;
	void setPolygon(const QPolygonF& polygon);

  vtkPolygon* getVtkPolygon() const;
	void applyVtkPolygonShape();

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/vtkpolygonactor_impl.h"
#endif // _DEBUG

#endif // VTKPOLYGONACTOR_H
