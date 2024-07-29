#ifndef ABSTRACTCROSSSECTIONWINDOW_GRAPHICSVIEW_ELEVATIONCHARTCONTROLLER_H
#define ABSTRACTCROSSSECTIONWINDOW_GRAPHICSVIEW_ELEVATIONCHARTCONTROLLER_H

#include "../public/abstractcrosssectionwindow_graphicsview.h"

class Structured2DGrid;

class AbstractCrosssectionWindow::GraphicsView::ElevationChartController
{
public:
	ElevationChartController(GraphicsView* view);

	void paint(const std::vector<double>& nodePositions, const std::vector<QVariant>& values,
										const std::vector<unsigned int>& selected,
										GridAttributeDisplaySettingContainer* setting,
										QMatrix& matrix, int elevationOffset, QPainter* painter);

private:
	QPointF mappedPos(unsigned int index, const std::vector<double>& nodePositions, const std::vector<QVariant>& values, const std::vector<unsigned int>& selected, int elevationOffset, const QMatrix& matrix);

	GraphicsView* m_graphicsView;
};

#endif // ABSTRACTCROSSSECTIONWINDOW_GRAPHICSVIEW_ELEVATIONCHARTCONTROLLER_H
