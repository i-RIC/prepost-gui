#ifndef PREPROCESSORGRIDCROSSSECTIONWINDOW2_GRAPHICSVIEW_INDEPENDENTCHARTCONTROLLER_H
#define PREPROCESSORGRIDCROSSSECTIONWINDOW2_GRAPHICSVIEW_INDEPENDENTCHARTCONTROLLER_H

#include "../public/preprocessorgridcrosssectionwindow2_graphicsview.h"

class Structured2DGrid;

class PreProcessorGridCrosssectionWindow2::GraphicsView::IndependentChartController
{
public:
	IndependentChartController(GraphicsView* view);

	void paint(const std::vector<double>& nodePositions, const std::vector<QVariant>& values,
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

#endif // PREPROCESSORGRIDCROSSSECTIONWINDOW2_GRAPHICSVIEW_INDEPENDENTCHARTCONTROLLER_H
