#ifndef VTKLINEACTOR_H
#define VTKLINEACTOR_H

#include "../guibase_global.h"

#include <vector>

class QPointF;

class vtkActor;
class vtkPolyData;

class GUIBASEDLL_EXPORT vtkLineActor
{
public:
	vtkLineActor();
	~vtkLineActor();

	vtkActor* pointsActor() const;
	vtkActor* lineActor() const;

	vtkPolyData* pointsPolyData() const;
	vtkPolyData* linesPolyData() const;

	std::vector<QPointF> line() const;
	void setLine(const std::vector<QPointF>& line);

private:
	class Impl;
	Impl* impl;
};

#endif // VTKLINEACTOR_H
