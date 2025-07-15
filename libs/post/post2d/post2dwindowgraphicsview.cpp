#include "post2dwindowgraphicsview.h"
#include "post2dwindowdatamodel.h"

Post2dWindowGraphicsView::Post2dWindowGraphicsView(QWidget* parent) :
	VTK2DGraphicsView(parent)
{
	setInteractive(true);
}

void Post2dWindowGraphicsView::viewportToWorld(double& x, double& y) const
{
	VTK2DGraphicsView::viewportToWorld(x, y);
	y /= model()->zScale();
}

QPointF Post2dWindowGraphicsView::viewportToWorld(const QPoint& p) const
{
	auto ret = VTK2DGraphicsView::viewportToWorld(p);
	ret.setY(ret.y() / model()->zScale());
	return ret;
}

void Post2dWindowGraphicsView::worldToViewport(double& x, double& y) const
{
	y *= model()->zScale();
	VTK2DGraphicsView::worldToViewport(x, y);
}

Post2dWindowDataModel* Post2dWindowGraphicsView::model() const
{
	return dynamic_cast<Post2dWindowDataModel*> (VTK2DGraphicsView::model());
}
