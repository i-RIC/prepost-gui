#ifndef PREPROCESSORGRAPHICSVIEWINTERFACE_H
#define PREPROCESSORGRAPHICSVIEWINTERFACE_H

#include "../../guicore_global.h"
#include "../../datamodel/vtk2dgraphicsview.h"

class GUICOREDLL_EXPORT PreProcessorGraphicsViewInterface : public VTK2DGraphicsView
{
public:
	PreProcessorGraphicsViewInterface(QWidget* widget);
	virtual ~PreProcessorGraphicsViewInterface();
};

#endif // PREPROCESSORGRAPHICSVIEWINTERFACE_H
