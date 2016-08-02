#ifndef POST2DWINDOWGRAPHICSVIEW_H
#define POST2DWINDOWGRAPHICSVIEW_H

#include <guicore/datamodel/vtk2dgraphicsview.h>

class Post2dWindowDataItem;
class Post2dWindowDataModel;
class vtkRenderer;
class QPixmap;
class QCursor;

class Post2dWindowGraphicsView : public VTK2DGraphicsView
{
	Q_OBJECT

public:
	Post2dWindowGraphicsView(QWidget* parent);

	void viewportToWorld(double& x, double& y) const override;
	void worldToViewport(double& x, double& y) const override;

private:
	Post2dWindowDataModel* model() const;
};

#endif // POST2DWINDOWGRAPHICSVIEW_H
