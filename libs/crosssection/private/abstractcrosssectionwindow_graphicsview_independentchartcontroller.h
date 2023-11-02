#ifndef ABSTRACTCROSSSECTIONWINDOW_GRAPHICSVIEW_INDEPENDENTCHARTCONTROLLER_H
#define ABSTRACTCROSSSECTIONWINDOW_GRAPHICSVIEW_INDEPENDENTCHARTCONTROLLER_H

#include "../public/abstractcrosssectionwindow_graphicsview.h"

class Structured2DGrid;

class AbstractCrosssectionWindow::GraphicsView::IndependentChartController
{
public:
	IndependentChartController(GraphicsView* view);

	void paintNode(const std::vector<double>& nodePositions, const std::vector<QVariant>& values,
										GridAttributeDisplaySettingContainer* setting,
										const DrawRegionInformation& info, QMatrix& matrix, QPainter* painter);
	void paintCell(const std::vector<double>& nodePositions, const std::vector<QVariant>& values,
										GridAttributeDisplaySettingContainer* setting,
										const DrawRegionInformation& info, QMatrix& matrix, QPainter* painter);

private:
	void drawNodeLine(const std::vector<double>& nodePositions, const std::vector<QVariant>& values,
										GridAttributeDisplaySettingContainer* setting,
										QMatrix& xMatrix, QMatrix& yMatrix, QPainter* painter);
	void drawCellLine(const std::vector<double>& nodePositions, const std::vector<QVariant>& values,
										GridAttributeDisplaySettingContainer* setting,
										QMatrix& xMatrix, QMatrix& yMatrix, QPainter* painter);
	void drawTitle(GridAttributeDisplaySettingContainer* setting,
								 const DrawRegionInformation& info, QPainter* painter);
	void drawScale(
			std::set<double> values, GridAttributeDisplaySettingContainer* setting,
			const DrawRegionInformation& info, const QMatrix& yMatrix, QPainter* painter);

	static QMatrix matrix(const DrawRegionInformation& info, double min, double max);

	GraphicsView* m_graphicsView;
};

#endif // ABSTRACTCROSSSECTIONWINDOW_GRAPHICSVIEW_INDEPENDENTCHARTCONTROLLER_H
