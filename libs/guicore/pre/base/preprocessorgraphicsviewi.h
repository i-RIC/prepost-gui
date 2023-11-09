#ifndef PREPROCESSORGRAPHICSVIEWI_H
#define PREPROCESSORGRAPHICSVIEWI_H

#include "../../guicore_global.h"
#include "../../datamodel/vtk2dgraphicsview.h"

class GUICOREDLL_EXPORT PreProcessorGraphicsViewI : public VTK2DGraphicsView
{
public:
	PreProcessorGraphicsViewI(QWidget* widget);
	virtual ~PreProcessorGraphicsViewI();
};

#endif // PREPROCESSORGRAPHICSVIEWI_H
