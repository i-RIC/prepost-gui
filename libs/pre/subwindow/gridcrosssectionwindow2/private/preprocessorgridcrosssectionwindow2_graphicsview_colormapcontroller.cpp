#include "preprocessorgridcrosssectionwindow2_graphicsview_colormapcontroller.h"
#include "preprocessorgridcrosssectionwindow2_graphicsview_drawregioninformation.h"
#include "preprocessorgridcrosssectionwindow2_gridattributedisplaysettingcontainer.h"
#include "preprocessorgridcrosssectionwindow2_impl.h"

#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>

namespace {

const int TITLE_HORIZONTAL_OFFSET = 10;
const int TITLE_VERTICAL_OFFSET = 5;
const int VERTICAL_OFFSET = 5;

} // namespace

PreProcessorGridCrosssectionWindow2::GraphicsView::ColorMapController::ColorMapController(GraphicsView* view) :
	m_graphicsView {view}
{}

void PreProcessorGridCrosssectionWindow2::GraphicsView::ColorMapController::paint(
		const std::vector<double>& nodePositions, const std::vector<QVariant>& values,
		GridAttributeDisplaySettingContainer* setting,
		const DrawRegionInformation& info, QMatrix& matrix, QPainter* painter)
{
	if (setting->attribute()->position() == SolverDefinitionGridAttribute::Position::Node) {
		drawNodeColorMap(nodePositions, values, setting, matrix, info, painter);
	} else {
		drawCellColorMap(nodePositions, values, setting, matrix, info, painter);
	}

	drawTitle(setting, info, painter);
}

void PreProcessorGridCrosssectionWindow2::GraphicsView::ColorMapController::drawNodeColorMap(
		const std::vector<double>& nodePositions, const std::vector<QVariant>& values,
		GridAttributeDisplaySettingContainer* setting,
		QMatrix& matrix, const DrawRegionInformation& info, QPainter* painter)
{
	ColorMapSettingContainerI* cm = setting->colorMapSetting;
	if (setting->usePreColor) {
		cm = m_graphicsView->m_impl->preColorMapSetting(setting->attribute()->name());
	}
	double y1 = info.yMin + VERTICAL_OFFSET;
	double y2 = info.yMax - VERTICAL_OFFSET;

	for (int i = 0; i < nodePositions.size() - 1; ++i) {
		auto p1x = matrix.map(QPointF(nodePositions.at(i), 0));
		auto p2x = matrix.map(QPointF(nodePositions.at(i + 1), 0));

		cm->paintNodeData(p1x.x(), p2x.x(), values.at(i).toDouble(), values.at(i + 1).toDouble(), y1, y2, painter);
	}
}

void PreProcessorGridCrosssectionWindow2::GraphicsView::ColorMapController::drawCellColorMap(
		const std::vector<double>& nodePositions, const std::vector<QVariant>& values,
		GridAttributeDisplaySettingContainer* setting,
		QMatrix& matrix, const DrawRegionInformation& info, QPainter* painter)
{
	ColorMapSettingContainerI* cm = setting->colorMapSetting;
	if (setting->usePreColor) {
		cm = m_graphicsView->m_impl->preColorMapSetting(setting->attribute()->name());
	}
	double y1 = info.yMin + VERTICAL_OFFSET;
	double y2 = info.yMax - VERTICAL_OFFSET;

	for (int i = 0; i < nodePositions.size() - 1; ++i) {
		auto v = values.at(i);
		if (v.isNull()) {
			continue;
		}
		auto p1x = matrix.map(QPointF(nodePositions.at(i), 0));
		auto p2x = matrix.map(QPointF(nodePositions.at(i + 1), 0));
		cm->paintCellData(p1x.x(), p2x.x(), v.toDouble(), y1, y2, painter);
	}
}

void PreProcessorGridCrosssectionWindow2::GraphicsView::ColorMapController::drawTitle(
		GridAttributeDisplaySettingContainer* setting,
		const DrawRegionInformation& info, QPainter* painter)
{
	if (! setting->colorMapTitleVisible) {return;}

	QFontMetrics metrics(setting->colorMapTitleFont);
	auto rect = metrics.boundingRect(setting->attribute()->caption());

	painter->save();
	painter->setPen(setting->colorMapTitleColor);
	painter->setFont(setting->colorMapTitleFont);

	QRectF textRect(m_graphicsView->width() - TITLE_HORIZONTAL_OFFSET - rect.width(), info.yMin,
								rect.width() + 1, setting->colorMapHeight);

	painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, setting->attribute()->caption());
	painter->restore();
}