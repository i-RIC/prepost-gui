#ifndef PREPROCESSORGRAPHICSVIEW_H
#define PREPROCESSORGRAPHICSVIEW_H

#include <guicore/datamodel/vtk2dgraphicsview.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>

class PreProcessorGraphicsView : public PreProcessorGraphicsViewInterface
{
public:
	PreProcessorGraphicsView(QWidget* parent);
	~PreProcessorGraphicsView() {}
};

#endif // PREPROCESSORGRAPHICSVIEW_H
