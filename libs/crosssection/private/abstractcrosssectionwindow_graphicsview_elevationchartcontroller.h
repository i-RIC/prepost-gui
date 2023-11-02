#ifndef ABSTRACTCROSSSECTIONWINDOW_GRAPHICSVIEW_ELEVATIONCHARTCONTROLLER_H
#define ABSTRACTCROSSSECTIONWINDOW_GRAPHICSVIEW_ELEVATIONCHARTCONTROLLER_H

#include "../public/abstractcrosssectionwindow_graphicsview.h"

class Structured2DGrid;

class AbstractCrosssectionWindow::GraphicsView::ElevationChartController
{
public:
	ElevationChartController(GraphicsView* view);

	void paint(const std::vector<double>& nodePositions, const std::vector<QVariant>& values,
										GridAttributeDisplaySettingContainer* setting,
										QMatrix& matrix, QPainter* painter);

private:
	GraphicsView* m_graphicsView;
};

#endif // ABSTRACTCROSSSECTIONWINDOW_GRAPHICSVIEW_ELEVATIONCHARTCONTROLLER_H
