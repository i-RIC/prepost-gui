#include "abstractcrosssectionwindow_graphicsview_independentchartcontroller.h"
#include "abstractcrosssectionwindow_graphicsview_drawregioninformation.h"
#include "abstractcrosssectionwindow_gridattributedisplaysettingcontainer.h"

#include <guicore/solverdef/solverdefinitiongridattribute.h>

namespace {

const int VERTICAL_MERGIN_PIXELS = 5;
const int VERTICAL_MERGIN_PIXELS2 = 5;
const double VERTICAL_MERGIN_RATIO = 0.1;
const int TITLE_HORIZONTAL_OFFSET = 10;
const int TITLE_VERTICAL_OFFSET = 5;
const double DELTA = 1.0E-10;

} // namespace

AbstractCrosssectionWindow::GraphicsView::IndependentChartController::IndependentChartController(GraphicsView* view) :
	m_graphicsView {view}
{}

void AbstractCrosssectionWindow::GraphicsView::IndependentChartController::paintNode(
		const std::vector<double>& nodePositions, const std::vector<QVariant>& values,
		GridAttributeDisplaySettingContainer* setting,
		const DrawRegionInformation& info, QMatrix& matrix, QPainter* painter)
{
	std::set<double> val;
	for (auto& v : values) {
		val.insert(v.toDouble());
	}

	double min = *val.begin();
	double max = *val.rbegin();

	auto yMatrix = this->matrix(info, min, max);

	drawScale(val, setting, info, yMatrix, painter);

	drawNodeLine(nodePositions, values, setting, matrix, yMatrix, painter);
	drawTitle(setting, info, painter);
}

void AbstractCrosssectionWindow::GraphicsView::IndependentChartController::paintCell(
		const std::vector<double>& nodePositions, const std::vector<QVariant>& values,
		GridAttributeDisplaySettingContainer* setting,
		const DrawRegionInformation& info, QMatrix& matrix, QPainter* painter)
{
	std::set<double> val;
	for (auto& v : values) {
		val.insert(v.toDouble());
	}

	double min = *val.begin();
	double max = *val.rbegin();

	auto yMatrix = this->matrix(info, min, max);

	drawScale(val, setting, info, yMatrix, painter);

	drawCellLine(nodePositions, values, setting, matrix, yMatrix, painter);
	drawTitle(setting, info, painter);
}

void AbstractCrosssectionWindow::GraphicsView::IndependentChartController::drawNodeLine(
		const std::vector<double>& nodePositions, const std::vector<QVariant>& values,
		GridAttributeDisplaySettingContainer* setting,
		QMatrix& xMatrix, QMatrix& yMatrix, QPainter* painter)
{
	painter->save();

	QPen pen(setting->independentChartLineColor.value(), setting->independentChartLineWidth);
	painter->setPen(pen);

	for (int i = 0; i < nodePositions.size() - 1; ++i) {
		auto p1x = xMatrix.map(QPointF(nodePositions.at(i), 0));
		auto p1y = yMatrix.map(QPointF(0, values.at(i).toDouble()));
		auto p2x = xMatrix.map(QPointF(nodePositions.at(i + 1), 0));
		auto p2y = yMatrix.map(QPointF(0, values.at(i + 1).toDouble()));

		painter->drawLine(QPointF(p1x.x(), p1y.y()), QPointF(p2x.x(), p2y.y()));
	}

	painter->restore();
}

void AbstractCrosssectionWindow::GraphicsView::IndependentChartController::drawCellLine(
		const std::vector<double>& nodePositions, const std::vector<QVariant>& values,
		GridAttributeDisplaySettingContainer* setting,
		QMatrix& xMatrix, QMatrix& yMatrix, QPainter* painter)
{
	painter->save();

	QPen pen(setting->independentChartLineColor.value(), setting->independentChartLineWidth);
	painter->setPen(pen);

	QPointF prevPy;
	for (int i = 0; i < nodePositions.size() - 1; ++i) {
		auto p1x = xMatrix.map(QPointF(nodePositions.at(i), 0));
		auto p2x = xMatrix.map(QPointF(nodePositions.at(i + 1), 0));
		auto v = values.at(i);
		if (v.isNull()) {
			prevPy = QPointF();
		} else {
			auto py = yMatrix.map(QPointF(0, v.toDouble()));
			painter->drawLine(QPointF(p1x.x(), py.y()), QPointF(p2x.x(), py.y()));
			if (! prevPy.isNull() && prevPy.y() != py.y()) {
				painter->drawLine(QPointF(p1x.x(), prevPy.y()), QPointF(p1x.x(), py.y()));
			}

			prevPy = py;
		}
	}

	painter->restore();
}

void AbstractCrosssectionWindow::GraphicsView::IndependentChartController::drawTitle(
		GridAttributeDisplaySettingContainer* setting,
		const DrawRegionInformation& info, QPainter* painter)
{
	if (! setting->independentChartTitleVisible) {return;}

	QFontMetrics metrics(setting->independentChartTitleFont);
	auto attName = setting->attributeName();
	auto caption = setting->attributeDataProvider()->caption(attName);
	auto rect = metrics.boundingRect(caption);

	painter->save();
	painter->setPen(setting->independentChartLineColor);
	painter->setFont(setting->independentChartTitleFont);

	QRectF textRect(m_graphicsView->width() - TITLE_HORIZONTAL_OFFSET - rect.width(), info.yMin + TITLE_VERTICAL_OFFSET,
								rect.width() + 1, rect.height());

	painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, caption);
	painter->restore();
}

void AbstractCrosssectionWindow::GraphicsView::IndependentChartController::drawScale(
		std::set<double> values, GridAttributeDisplaySettingContainer* setting,
		const DrawRegionInformation& info, const QMatrix& yMatrix, QPainter* painter)
{
	double xoffset = 5;
	double mainruler = 5;
	double labeloffset = 3;

	painter->save();

	QPen pen(setting->independentChartAxisColor);
	painter->setPen(pen);
	QPointF from(xoffset, info.yMin + VERTICAL_MERGIN_PIXELS);
	QPointF to(xoffset, info.yMax - VERTICAL_MERGIN_PIXELS);
	painter->drawLine(from, to);

	painter->setFont(setting->independentChartAxisFont);

	// draw labels
	QFontMetrics metrics(setting->independentChartAxisFont);
	for (auto v : values) {
		auto p = yMatrix.map(QPointF(0, v));
		painter->drawLine(QPointF(xoffset, p.y()), QPointF(xoffset + mainruler, p.y()));
		auto label = QString("%1").arg(v);
		auto fontRect = metrics.boundingRect(label);

		QRectF rect(xoffset + mainruler + labeloffset, p.y() - fontRect.height() * 0.5, fontRect.width() + 2, fontRect.height() + 2);
		painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, label);
	}

	painter->restore();
}

QMatrix AbstractCrosssectionWindow::GraphicsView::IndependentChartController::matrix(const DrawRegionInformation& info, double min, double max)
{
	double centerVal = (min + max) * 0.5;
	double scaleY = 1;
	if ((max - min) > DELTA) {
		scaleY = (info.yMax - info.yMin - 2 * VERTICAL_MERGIN_PIXELS - 2 * VERTICAL_MERGIN_PIXELS2) / ((max - min) * (1 + VERTICAL_MERGIN_RATIO * 2));
	}
	double centerPos = (info.yMin + info.yMax) * 0.5;

	QMatrix translate1(1, 0, 0, 1, 0, - centerVal);
	QMatrix scale(1, 0, 0, - scaleY, 0, 0);
	QMatrix translate2(1, 0, 0, 1, 0, centerPos);

	return translate1 * scale * translate2;
}
