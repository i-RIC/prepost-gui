#ifndef PREPROCESSORGRAPHICSVIEWINTERFACE_H
#define PREPROCESSORGRAPHICSVIEWINTERFACE_H

#include "../../guicore_global.h"
#include "../../datamodel/vtk2dgraphicsview.h"

class GUICOREDLL_EXPORT PreProcessorGraphicsViewInterface : public VTK2DGraphicsView
{
public:
	/// constructor
	PreProcessorGraphicsViewInterface(QWidget* widget);
	/// destructor
	~PreProcessorGraphicsViewInterface(){}
};

#endif // PREPROCESSORGRAPHICSVIEWINTERFACE_H
