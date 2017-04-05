#ifndef VTKLINESACTOR_H
#define VTKLINESACTOR_H

#include "../guibase_global.h"

#include <vector>

class QPointF;

class vtkActor;
class vtkPolyData;

class GUIBASEDLL_EXPORT vtkLinesActor
{
public:
	vtkLinesActor();
	~vtkLinesActor();

	vtkPolyData* pointsPolyData() const;
	vtkPolyData* linesPolyData() const;

	vtkActor* pointsActor() const;
	vtkActor* linesActor() const;

	void setLines(const std::vector<std::vector<QPointF> >& lines);

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/vtklinesactor_impl.h"
#endif // _DEBUG

#endif // VTKLINESACTOR_H
