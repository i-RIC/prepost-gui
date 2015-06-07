#ifndef POST2DBIRDEYEWINDOWGRAPHICSVIEW_H
#define POST2DBIRDEYEWINDOWGRAPHICSVIEW_H

#include <guicore/datamodel/vtk3dgraphicsview.h>

class Post2dBirdEyeWindowDataItem;
class vtkRenderer;
class QPixmap;
class QCursor;

class Post2dBirdEyeWindowGraphicsView : public VTK3DGraphicsView
{
	Q_OBJECT

public:
	Post2dBirdEyeWindowGraphicsView(QWidget* parent)
		: VTK3DGraphicsView(parent)
	{}
};

#endif // POST2DBIRDEYEWINDOWGRAPHICSVIEW_H
