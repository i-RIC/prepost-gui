#ifndef POST3DWINDOWGRAPHICSVIEW_H
#define POST3DWINDOWGRAPHICSVIEW_H

#include <guicore/datamodel/vtk3dgraphicsview.h>

class Post3dWindowDataItem;
class vtkRenderer;
class QPixmap;
class QCursor;

class Post3dWindowGraphicsView : public VTK3DGraphicsView
{
public:
	Post3dWindowGraphicsView(QWidget* parent)
		: VTK3DGraphicsView(parent)
	{}
};

#endif // POST3DWINDOWGRAPHICSVIEW_H
