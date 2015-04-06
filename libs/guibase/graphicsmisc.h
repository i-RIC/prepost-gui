#ifndef GRAPHICSMISC_H
#define GRAPHICSMISC_H

#include "guibase_global.h"

class QPixmap;
class QSize;

class vtkImageData;
class vtkCubeAxesActor2D;
class vtkAxesActor;
class vtkScalarBarActor;

namespace iRIC {
	QPixmap GUIBASEDLL_EXPORT pixmapFromVTKImage(QSize size, vtkImageData* image);
	void GUIBASEDLL_EXPORT fattenBounds(double bounds[6], double rate);
	void GUIBASEDLL_EXPORT setupCubeActor2D(vtkCubeAxesActor2D* actor);
	void GUIBASEDLL_EXPORT setupAxesActor(vtkAxesActor* actor);
	void GUIBASEDLL_EXPORT setupScalarBarProperty(vtkScalarBarActor* actor);
	void GUIBASEDLL_EXPORT setupAxesActor2D();
}

#endif // GRAPHICSMISC_H
