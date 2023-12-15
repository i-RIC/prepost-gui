#include "abstractcrosssectionwindow_gridattributedisplaysettingcontainer.h"
#include "abstractcrosssectionwindow_graphicsview_elevationchartcontroller.h"

#include <QFontMetrics>
#include <QPen>

namespace {

const int INDEX_HOFFSET = 5;
const int INDEX_VOFFSET = 5;

} // namespace

AbstractCrosssectionWindow::GraphicsView::ElevationChartController::ElevationChartController(GraphicsView* view) :
	m_graphicsView {view}
{}

void AbstractCrosssectionWindow::GraphicsView::ElevationChartController::paint(
		const std::vector<double>& nodePositions, const std::vector<QVariant>& values,
		GridAttributeDisplaySettingContainer* setting,
		QMatrix& matrix, QPainter* painter)
{
	painter->save();
	QPen pen(setting->elevationChartLineColor.value(), setting->elevationChartLineWidth);
	painter->setPen(pen);

	for (int i = 0; i < static_cast<int> (nodePositions.size()) - 1; ++i) {
		QPointF p1(nodePositions.at(i), values.at(i).toDouble());
		QPointF p2(nodePositions.at(i + 1), values.at(i + 1).toDouble());

		painter->drawLine(matrix.map(p1), matrix.map(p2));
	}

	if (setting->elevationChartShowPoint) {
		QBrush brush(setting->elevationChartLineColor);
		painter->setBrush(brush);
		double pointSize = setting->elevationChartPointSize * 0.5;
		QPointF offset(pointSize, pointSize);

		for (int i = 0; i < nodePositions.size(); ++i) {
			QPointF p1 = matrix.map(QPointF(nodePositions.at(i), values.at(i).toDouble()));
			QRectF rect(p1 - offset, p1 + offset);
			painter->drawEllipse(rect);
		}
	}

	if (setting->elevationChartShowIndex) {
		painter->setFont(setting->elevationChartIndexFont);
		QFontMetrics metrics(setting->elevationChartIndexFont);

		for (int i = 0; i < static_cast<int> (nodePositions.size()); ++i) {
			QPointF p1(nodePositions.at(i), values.at(i).toDouble());
			auto p2 = matrix.map(p1) + QPointF(INDEX_HOFFSET, INDEX_VOFFSET);

			if (i % setting->elevationChartIndexSkipRate == 0) {
				auto str = QString("%1").arg(i + 1);
				auto rect = metrics.boundingRect(str);
				QRectF fontRect(p2.x(), p2.y(), rect.width() + 5, rect.height() + 5);
				painter->drawText(fontRect, Qt::AlignLeft |Qt::AlignTop, str);
			}
		}
	}
	painter->restore();
}
