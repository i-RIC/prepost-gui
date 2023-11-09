#ifndef PREPROCESSORGRAPHICSVIEW_H
#define PREPROCESSORGRAPHICSVIEW_H

#include <guicore/datamodel/vtk2dgraphicsview.h>
#include <guicore/pre/base/preprocessorgraphicsviewi.h>

class PreProcessorGraphicsView : public PreProcessorGraphicsViewI
{

public:
	PreProcessorGraphicsView(QWidget* parent);
	~PreProcessorGraphicsView();
};

#endif // PREPROCESSORGRAPHICSVIEW_H
