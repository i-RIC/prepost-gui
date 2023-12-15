#include "abstractcrosssectionwindow_graphicsview_colormapcontroller.h"
#include "abstractcrosssectionwindow_graphicsview_drawregioninformation.h"
#include "abstractcrosssectionwindow_gridattributedisplaysettingcontainer.h"
#include "abstractcrosssectionwindow_impl.h"

#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>

namespace {

const int TITLE_HORIZONTAL_OFFSET = 10;
const int TITLE_VERTICAL_OFFSET = 5;
const int VERTICAL_OFFSET = 5;

} // namespace

AbstractCrosssectionWindow::GraphicsView::ColorMapController::ColorMapController(GraphicsView* view) :
	m_graphicsView {view}
{}

void AbstractCrosssectionWindow::GraphicsView::ColorMapController::paintNode(
		const std::vector<double>& nodePositions, const std::vector<QVariant>& values,
		GridAttributeDisplaySettingContainer* setting,
		const DrawRegionInformation& info, QMatrix& matrix, QPainter* painter)
{
	drawNodeColorMap(nodePositions, values, setting, matrix, info, painter);
	drawTitle(setting, info, painter);
}

void AbstractCrosssectionWindow::GraphicsView::ColorMapController::paintCell(
		const std::vector<double>& nodePositions, const std::vector<QVariant>& values,
		GridAttributeDisplaySettingContainer* setting,
		const DrawRegionInformation& info, QMatrix& matrix, QPainter* painter)
{
	drawCellColorMap(nodePositions, values, setting, matrix, info, painter);
	drawTitle(setting, info, painter);
}

void AbstractCrosssectionWindow::GraphicsView::ColorMapController::drawNodeColorMap(
		const std::vector<double>& nodePositions, const std::vector<QVariant>& values,
		GridAttributeDisplaySettingContainer* setting,
		QMatrix& matrix, const DrawRegionInformation& info, QPainter* painter)
{
	ColorMapSettingContainerI* cm = setting->colorMapSetting;
	if (setting->usePreColor) {
		cm = m_graphicsView->m_impl->m_window->preColorMapSetting(setting->attributeName());
	}
	double y1 = info.yMin + VERTICAL_OFFSET;
	double y2 = info.yMax - VERTICAL_OFFSET;

	for (int i = 0; i < nodePositions.size() - 1; ++i) {
		auto p1x = matrix.map(QPointF(nodePositions.at(i), 0));
		auto p2x = matrix.map(QPointF(nodePositions.at(i + 1), 0));

		cm->paintNodeData(p1x.x(), p2x.x(), values.at(i).toDouble(), values.at(i + 1).toDouble(), y1, y2, painter);
	}
}

void AbstractCrosssectionWindow::GraphicsView::ColorMapController::drawCellColorMap(
		const std::vector<double>& nodePositions, const std::vector<QVariant>& values,
		GridAttributeDisplaySettingContainer* setting,
		QMatrix& matrix, const DrawRegionInformation& info, QPainter* painter)
{
	ColorMapSettingContainerI* cm = setting->colorMapSetting;
	if (setting->usePreColor) {
		cm = m_graphicsView->m_impl->m_window->preColorMapSetting(setting->attributeName());
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

void AbstractCrosssectionWindow::GraphicsView::ColorMapController::drawTitle(
		GridAttributeDisplaySettingContainer* setting,
		const DrawRegionInformation& info, QPainter* painter)
{
	if (! setting->colorMapTitleVisible) {return;}

	QFontMetrics metrics(setting->colorMapTitleFont);
	auto attName = setting->attributeName();
	auto caption = setting->attributeDataProvider()->caption(attName);
	auto rect = metrics.boundingRect(caption);

	painter->save();
	painter->setPen(setting->colorMapTitleColor);
	painter->setFont(setting->colorMapTitleFont);

	QRectF textRect(m_graphicsView->width() - TITLE_HORIZONTAL_OFFSET - rect.width(), info.yMin,
								rect.width() + 1, setting->colorMapHeight);

	painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, caption);
	painter->restore();
}
