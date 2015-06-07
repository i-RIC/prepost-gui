#include "post2dwindowgraphicsview.h"

Post2dWindowGraphicsView::Post2dWindowGraphicsView(QWidget* parent)
	: VTK2DGraphicsView(parent)
{
	setInteractive(true);
}
