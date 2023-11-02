#ifndef ABSTRACTCROSSSECTIONWINDOW_GRAPHICSVIEW_DRAWREGIONINFORMATION_H
#define ABSTRACTCROSSSECTIONWINDOW_GRAPHICSVIEW_DRAWREGIONINFORMATION_H

#include "../public/abstractcrosssectionwindow_graphicsview.h"

#include <QMatrix>

class AbstractCrosssectionWindow::GraphicsView::DrawRegionInformation
{
public:
	DrawRegionInformation();

	int yMin;
	int yMax;
};

#endif // ABSTRACTCROSSSECTIONWINDOW_GRAPHICSVIEW_DRAWREGIONINFORMATION_H
