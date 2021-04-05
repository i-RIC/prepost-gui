#ifndef VTKARROWLEGENDACTORS_H
#define VTKARROWLEGENDACTORS_H

#include "../guibase_global.h"

#include <string>

class vtkActor2D;
class vtkTextActor;

class GUIBASEDLL_EXPORT vtkArrowLegendActors
{
public:
	vtkArrowLegendActors();
	~vtkArrowLegendActors();

	void update(const std::string& name, double pixels, double length, double headLen = 8, double angle = 15);

	void setPosition(double xpos, double ypos);
	void setColor(double r, double g, double b);
	void setColor(double* rgb);
	void setLineWidth(double lineWidth);

	vtkTextActor* textActor() const;
	vtkActor2D* arrowActor() const;

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/vtkarrowlegendactors_impl.h"
#endif // _DEBUG

#endif // VTKARROWLEGENDACTORS_H
