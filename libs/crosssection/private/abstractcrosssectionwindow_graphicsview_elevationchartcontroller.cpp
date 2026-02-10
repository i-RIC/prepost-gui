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

void AbstractCrosssectionWindow::GraphicsView::ElevationChartController::paintNode(
		const std::vector<double>& nodePositions, const std::vector<QVariant>& values,
		const std::vector<unsigned int>& selected,
		GridAttributeDisplaySettingContainer* setting,
		QMatrix& matrix, int elevationOffset, QPainter* painter)
{
	painter->save();
	QPen pen(setting->elevationChartLineColor.value(), setting->elevationChartLineWidth);
	painter->setPen(pen);

	for (int i = 0; i < static_cast<int> (nodePositions.size()) - 1; ++i) {
		QPointF p1 = mappedPos(i, nodePositions, values, selected, elevationOffset, matrix);
		QPointF p2 = mappedPos(i + 1, nodePositions, values, selected, elevationOffset, matrix);

		painter->drawLine(p1, p2);
	}

	if (setting->elevationChartShowPoint) {
		QBrush brush(setting->elevationChartLineColor);
		painter->setBrush(brush);
		double pointSize = setting->elevationChartPointSize * 0.5;
		QPointF offset(pointSize, pointSize);

		for (int i = 0; i < nodePositions.size(); ++i) {
			QPointF p1 = mappedPos(i, nodePositions, values, selected, elevationOffset, matrix);
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

	// show selected points
	QBrush brush(setting->elevationChartLineColor);
	painter->setBrush(brush);
	double pointSize = setting->elevationChartPointSize * 0.5 + 2;
	QPointF offset(pointSize, pointSize);

	for (auto i : selected) {
		QPointF p1 = mappedPos(i, nodePositions, values, selected, elevationOffset, matrix);
		QRectF rect(p1 - offset, p1 + offset);
		painter->drawEllipse(rect);
	}

	painter->restore();
}

void AbstractCrosssectionWindow::GraphicsView::ElevationChartController::paintCell(
	const std::vector<double>& nodePositions, const std::vector<QVariant>& values,
	const std::vector<unsigned int>& selected,
	GridAttributeDisplaySettingContainer* setting,
	QMatrix& matrix, int elevationOffset, QPainter* painter)
{
	painter->save();
	QPen pen(setting->elevationChartLineColor.value(), setting->elevationChartLineWidth);
	painter->setPen(pen);

	for (int i = 0; i < nodePositions.size() - 1; ++i) {
		QPointF p1 = mappedPos(i, nodePositions, values, selected, elevationOffset, matrix);
		QPointF p2;
		if (i + 1 < values.size()) {
			p2 = mappedPos(i + 1, nodePositions, values, selected, elevationOffset, matrix);
			painter->drawLine(QPoint(p2.x(), p1.y()), QPoint(p2.x(), p2.y()));
		} else {
			p2 = matrix.map(QPointF(nodePositions.at(i + 1), -1));
		}
		painter->drawLine(p1, QPoint(p2.x(), p1.y()));
	}

	if (setting->elevationChartShowPoint) {
		QBrush brush(setting->elevationChartLineColor);
		painter->setBrush(brush);
		double pointSize = setting->elevationChartPointSize * 0.5;
		QPointF offset(pointSize, pointSize);

		for (int i = 0; i < static_cast<int> (nodePositions.size()) - 1; ++i) {
			QPointF p1 = mappedPos(i, nodePositions, values, selected, elevationOffset, matrix);
			QPointF p2;
			if (i + 1 < values.size()) {
				p2 = mappedPos(i + 1, nodePositions, values, selected, elevationOffset, matrix);

			} else {
				p2 = matrix.map(QPointF(nodePositions.at(i + 1), -1));
			}
			QPointF CellPoint = QPointF((p1.x() + p2.x()) / 2, p1.y());
			QRectF rect(CellPoint - offset, CellPoint + offset);
			painter->drawEllipse(rect);
		}
	}

	if (setting->elevationChartShowIndex) {
		painter->setFont(setting->elevationChartIndexFont);
		QFontMetrics metrics(setting->elevationChartIndexFont);

		for (int i = 0; i < static_cast<int> (nodePositions.size()) - 1; ++i) {
			QPointF p1(nodePositions.at(i), values.at(i).toDouble());
			if (i + 1 < values.size()) {
				QPointF p2(nodePositions.at(i + 1), values.at(i + 1).toDouble());

				QPointF CellPoint = QPointF((matrix.map(p1).x() + matrix.map(p2).x()) / 2, matrix.map(p1).y());
				auto IndexPoint = CellPoint + QPointF(INDEX_HOFFSET, INDEX_VOFFSET);

				if (i % setting->elevationChartIndexSkipRate == 0) {
					auto str = QString("%1").arg(i + 1);
					auto rect = metrics.boundingRect(str);
					QRectF fontRect(IndexPoint.x(), IndexPoint.y(), rect.width() + 5, rect.height() + 5);
					painter->drawText(fontRect, Qt::AlignLeft | Qt::AlignTop, str);
				}
			}
		}
	}

	// show selected points
	QBrush brush(setting->elevationChartLineColor);
	painter->setBrush(brush);
	double pointSize = setting->elevationChartPointSize * 0.5 + 2;
	QPointF offset(pointSize, pointSize);

	for (auto i : selected) {
		QPointF p1 = mappedPos(i, nodePositions, values, selected, elevationOffset, matrix);
		QRectF rect(p1 - offset, p1 + offset);
		painter->drawEllipse(rect);
	}

	painter->restore();
}

QPointF AbstractCrosssectionWindow::GraphicsView::ElevationChartController::mappedPos(unsigned int index, const std::vector<double>& nodePositions, const std::vector<QVariant>& values, const std::vector<unsigned int>& selected, int elevationOffset, const QMatrix& matrix)
{
	double x = nodePositions.at(index);
	double y = values.at(index).toDouble();

	auto ret = matrix.map(QPointF(x, y));
	auto it = std::find(selected.begin(), selected.end(), index);

	if (it != selected.end()) {
		ret = QPoint(ret.x(), ret.y() + elevationOffset);
	}

	return ret;
}
