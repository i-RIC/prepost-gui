#ifndef VTKLINEACTOR_H
#define VTKLINEACTOR_H

#include "../guibase_global.h"

#include <vector>
#include <QPointF>

class vtkActor;

class GUIBASEDLL_EXPORT vtkLineActor
{
public:
	vtkLineActor();
	~vtkLineActor();

	vtkActor* pointsActor() const;
	vtkActor* lineActor() const;

	void setLine(const std::vector<QPointF>& line);
	std::vector<QPointF> line() const;

private:
	class Impl;
	Impl* impl;
};

#endif // VTKLINEACTOR_H
