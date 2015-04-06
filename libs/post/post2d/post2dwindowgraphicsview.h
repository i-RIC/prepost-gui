#ifndef POST2DWINDOWGRAPHICSVIEW_H
#define POST2DWINDOWGRAPHICSVIEW_H

#include <guicore/datamodel/vtk2dgraphicsview.h>

class Post2dWindowDataItem;
class vtkRenderer;
class QPixmap;
class QCursor;

class Post2dWindowGraphicsView : public VTK2DGraphicsView
{
	Q_OBJECT
public:
	Post2dWindowGraphicsView(QWidget* parent);
};

#endif // POST2DWINDOWGRAPHICSVIEW_H
