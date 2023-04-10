#ifndef PREPROCESSORGRIDCROSSSECTIONWINDOW2_GRAPHICSVIEW_DRAWREGIONINFORMATION_H
#define PREPROCESSORGRIDCROSSSECTIONWINDOW2_GRAPHICSVIEW_DRAWREGIONINFORMATION_H

#include "../public/preprocessorgridcrosssectionwindow2_graphicsview.h"

#include <QMatrix>

class PreProcessorGridCrosssectionWindow2::GraphicsView::DrawRegionInformation
{
public:
	DrawRegionInformation();

	int yMin;
	int yMax;
};

#endif // PREPROCESSORGRIDCROSSSECTIONWINDOW2_GRAPHICSVIEW_DRAWREGIONINFORMATION_H
