#ifndef VTKARROWSACTOR_H
#define VTKARROWSACTOR_H

#include "../guibase_global.h"

#include <string>

class vtkActor;
class vtkPolyData;
class vtkScalarsToColors;

class GUIBASEDLL_EXPORT vtkArrowsActor
{
public:
	vtkArrowsActor();
	~vtkArrowsActor();

	void setPolyData(vtkPolyData* polyData);
	void setAngle(double angle);
	void setLineWidth(double width);
	void setConeHeight(double height);
	void setScaleFactor(double scaleFactor);

	void setColorModeToCustom(double r, double g, double b);
	void setColorModeToCustom(double* rgb);
	void setColorModeToValue(const std::string& target, vtkScalarsToColors* scalarsToColors);

	vtkActor* actor() const;

private:
	class Impl;
	Impl* impl;
};

#if _DEBUG
	#include "private/vtkarrowsactor_impl.h"
#endif // _DEBUG

#endif // VTKARROWSACTOR_H
