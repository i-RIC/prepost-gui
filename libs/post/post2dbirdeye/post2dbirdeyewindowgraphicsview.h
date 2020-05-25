#ifndef POST2DBIRDEYEWINDOWGRAPHICSVIEW_H
#define POST2DBIRDEYEWINDOWGRAPHICSVIEW_H

#include <guicore/datamodel/vtk3dgraphicsview.h>

class Post2dBirdEyeWindowDataItem;
class vtkRenderer;
class QPixmap;
class QCursor;

class Post2dBirdEyeWindowGraphicsView : public VTK3DGraphicsView
{
public:
	Post2dBirdEyeWindowGraphicsView(QWidget* parent);
};

#endif // POST2DBIRDEYEWINDOWGRAPHICSVIEW_H
