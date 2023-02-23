#ifndef PREPROCESSORGRIDCROSSSECTIONWINDOW2_GRAPHICSVIEW_COLORMAPCONTROLLER_H
#define PREPROCESSORGRIDCROSSSECTIONWINDOW2_GRAPHICSVIEW_COLORMAPCONTROLLER_H

#include "../public/preprocessorgridcrosssectionwindow2_graphicsview.h"

class Structured2DGrid;

class PreProcessorGridCrosssectionWindow2::GraphicsView::ColorMapController
{
public:
	ColorMapController(GraphicsView* view);

	void paint(const std::vector<double>& nodePositions, const std::vector<QVariant>& values,
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

#endif // PREPROCESSORGRIDCROSSSECTIONWINDOW2_GRAPHICSVIEW_COLORMAPCONTROLLER_H
