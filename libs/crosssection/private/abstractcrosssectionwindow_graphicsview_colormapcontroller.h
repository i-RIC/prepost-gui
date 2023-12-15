#ifndef ABSTRACTCROSSSECTIONWINDOW_GRAPHICSVIEW_COLORMAPCONTROLLER_H
#define ABSTRACTCROSSSECTIONWINDOW_GRAPHICSVIEW_COLORMAPCONTROLLER_H

#include "../public/abstractcrosssectionwindow_graphicsview.h"

class Structured2DGrid;

class AbstractCrosssectionWindow::GraphicsView::ColorMapController
{
public:
	ColorMapController(GraphicsView* view);

	void paintNode(const std::vector<double>& nodePositions, const std::vector<QVariant>& values,
						 GridAttributeDisplaySettingContainer* setting,
						 const DrawRegionInformation& info, QMatrix& matrix, QPainter* painter);
	void paintCell(const std::vector<double>& nodePositions, const std::vector<QVariant>& values,
						 GridAttributeDisplaySettingContainer* setting,
						 const DrawRegionInformation& info, QMatrix& matrix, QPainter* painter);

private:
	void drawNodeColorMap(const std::vector<double>& nodePositions, const std::vector<QVariant>& values,
												GridAttributeDisplaySettingContainer* setting,
												QMatrix& matrix, const DrawRegionInformation& info, QPainter* painter);
	void drawCellColorMap(const std::vector<double>& nodePositions, const std::vector<QVariant>& values,
												GridAttributeDisplaySettingContainer* setting,
												QMatrix& matrix, const DrawRegionInformation& info, QPainter* painter);
	void drawTitle(GridAttributeDisplaySettingContainer* setting,
								 const DrawRegionInformation& info, QPainter* painter);

	GraphicsView* m_graphicsView;
};

#endif // ABSTRACTCROSSSECTIONWINDOW_GRAPHICSVIEW_COLORMAPCONTROLLER_H
