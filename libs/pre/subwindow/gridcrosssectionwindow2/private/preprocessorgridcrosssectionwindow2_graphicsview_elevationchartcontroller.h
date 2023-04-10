#ifndef PREPROCESSORGRIDCROSSSECTIONWINDOW2_GRAPHICSVIEW_ELEVATIONCHARTCONTROLLER_H
#define PREPROCESSORGRIDCROSSSECTIONWINDOW2_GRAPHICSVIEW_ELEVATIONCHARTCONTROLLER_H

#include "../public/preprocessorgridcrosssectionwindow2_graphicsview.h"

class Structured2DGrid;

class PreProcessorGridCrosssectionWindow2::GraphicsView::ElevationChartController
{
public:
	ElevationChartController(GraphicsView* view);

	void paint(const std::vector<double>& nodePositions, const std::vector<QVariant>& values,
										GridAttributeDisplaySettingContainer* setting,
										QMatrix& matrix, QPainter* painter);

private:
	GraphicsView* m_graphicsView;
};

#endif // PREPROCESSORGRIDCROSSSECTIONWINDOW2_GRAPHICSVIEW_ELEVATIONCHARTCONTROLLER_H
