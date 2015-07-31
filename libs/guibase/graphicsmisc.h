#ifndef GRAPHICSMISC_H
#define GRAPHICSMISC_H

#include "guibase_global.h"

class QPixmap;
class QSize;

class vtkAxesActor;
class vtkCubeAxesActor2D;
class vtkImageData;
class vtkScalarBarActor;
class vtkTextProperty;

namespace iRIC
{
	/// Get QPixmap instance from vtkImageData
	QPixmap GUIBASEDLL_EXPORT pixmapFromVTKImage(QSize size, vtkImageData* image);
	/**
	 * @brief Fatten the bounds by the specified rate.
	 *
	 * For example, if rate is 0.2, the bounds "fattens" in 20% for both horizontal and vertcical direction.
	 */
	void GUIBASEDLL_EXPORT fattenBounds(double bounds[6], double rate);
	/// Setup the vtkCubeAxesActor2D instance into the standard format used in iRIC
	void GUIBASEDLL_EXPORT setupCubeActor2D(vtkCubeAxesActor2D* actor);
	/// Setup the vtkAxesActor instance into the standard format used in iRIC
	void GUIBASEDLL_EXPORT setupAxesActor(vtkAxesActor* actor);
	/// Setup the vtkScalarBarActor instance properties into the standard one used in iRIC
	void GUIBASEDLL_EXPORT setupScalarBarProperty(vtkScalarBarActor* actor);
	/// Setup the vtkTextProperty into the standard one used in iRIC
	void GUIBASEDLL_EXPORT setupGridIndexTextProperty(vtkTextProperty* property);
}

#endif // GRAPHICSMISC_H
