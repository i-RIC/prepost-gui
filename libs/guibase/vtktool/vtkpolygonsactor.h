#ifndef VTKPOLYGONSACTOR_H
#define VTKPOLYGONSACTOR_H

#include "../guibase_global.h"

#include <vector>

class QPolygonF;

class vtkActor;
class vtkPolyData;

class GUIBASEDLL_EXPORT vtkPolygonsActor
{
public:
	vtkPolygonsActor();
	~vtkPolygonsActor();

	vtkActor* pointsActor();
	vtkActor* linesActor();
	vtkActor* paintActor();

	vtkPolyData* pointsPolyData() const;
	vtkPolyData* linesPolyData() const;
	vtkPolyData* polygonsPolyData() const;

	void setPolygons(const std::vector<QPolygonF>& polygons);
	void setPolygon(const QPolygonF& polygon);

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/vtkpolygonsactor_impl.h"
#endif // _DEBUG

#endif // VTKPOLYGONSACTOR_H
