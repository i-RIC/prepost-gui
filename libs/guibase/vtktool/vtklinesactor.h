#ifndef VTKLINESACTOR_H
#define VTKLINESACTOR_H

#include "../guibase_global.h"

#include <vector>
#include <QPointF>

class vtkActor;

class GUIBASEDLL_EXPORT vtkLinesActor
{
public:
	vtkLinesActor();
	~vtkLinesActor();

	vtkActor* pointsActor() const;
	vtkActor* linesActor() const;

	void setLines(const std::vector<std::vector<QPointF> >& lines);

private:
	class Impl;
	Impl* impl;
};

#endif // VTKLINESACTOR_H
