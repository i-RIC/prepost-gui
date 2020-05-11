#include "crosssectionwindow.h"
#include "crosssectionwindowgraphicsview.h"
#include "../../data/crosssection/crosssection.h"
#include "../../data/project/project.h"

#include <QFont>
#include <QPainter>

namespace {

int ellipseR = 3;
int HWM_RIGHT_MARGIN = 10;
int HWM_BOTTOM_MARGIN = 6;

enum Position {
	Left,
	Right
};

QPointF toP(const QVector2D& v)
{
	return QPointF(v.x(), v.y());
}

void drawHWM(const QString& label, double y, Position pos, QWidget* widget, QPainter* painter)
{
	painter->drawLine(QPointF(0, y), QPointF(widget->width(), y));

	QFontMetrics metrics(painter->font());
	auto rect = metrics.boundingRect(label);
	double left;
	if (pos == Left) {
		left = HWM_RIGHT_MARGIN;
	} else if (pos == Right) {
		left = widget->width() - HWM_RIGHT_MARGIN - rect.width();
	}
	auto top = y - HWM_BOTTOM_MARGIN - rect.height();
	QRectF fontRect(left, top, rect.width() + 1, rect.height() + 1);
	painter->drawText(fontRect, label);
}

} // namespace

CrossSectionWindowGraphicsView::CrossSectionWindowGraphicsView(QWidget* parent) :
	ChartGraphicsView(parent)
{}

const CrossSectionWindowDisplaySetting& CrossSectionWindowGraphicsView::crossSectionWindowDisplaySetting() const
{
	return m_crossSectionWindowDisplaySetting;
}

void CrossSectionWindowGraphicsView::setCrossSectionWindowDisplaySetting(const CrossSectionWindowDisplaySetting& setting)
{
	m_crossSectionWindowDisplaySetting = setting;
}

void CrossSectionWindowGraphicsView::paintEvent(QPaintEvent* /*event*/)
{
	QPainter painter(this);
	draw(&painter);
}

void CrossSectionWindowGraphicsView::draw(QPainter* painter)
{
	QRect r = rect();
	auto matrix = getMatrix(r);

	paintBackground(painter);
	drawScales(painter, matrix);
	drawAspectRatio(painter);
	drawHWMs(painter, matrix);
	drawLines(painter, matrix);
	drawCircles(painter, matrix);
}

void CrossSectionWindowGraphicsView::drawHWMs(QPainter* painter, const QMatrix& matrix)
{
	painter->save();

	auto cw = chartWindow();
	auto cs = cw->currentCrossSection();
	double w = width();

	painter->setFont(m_crossSectionWindowDisplaySetting.HWMFont);
	painter->setPen(m_crossSectionWindowDisplaySetting.WSEColor);
	auto p = matrix.map(QPointF(0, cs->waterElevation()));

	if (cw->showWSE()) {
		painter->drawLine(QPointF(0, p.y()), QPointF(width(), p.y()));
	}

	painter->setPen(QPen(QBrush(m_crossSectionWindowDisplaySetting.HWMColor), 1, Qt::DashLine));

	double lbhwm = cw->project()->calcLeftBankHWMAtCrossSection(cs);
	if (lbhwm != Project::INVALID_HWM && cw->showLBHWM()) {
		auto p = matrix.map(QPointF(0, lbhwm));
		drawHWM(tr("Left bank HWM"), p.y(), Left, this, painter);
	}
	double rbhwm = cw->project()->calcRightBankHWMAtCrossSection(cs);
	if (rbhwm != Project::INVALID_HWM && cw->showRBHWM()) {
		auto p = matrix.map(QPointF(0, rbhwm));
		drawHWM(tr("Right bank HWM"), p.y(), Right, this, painter);
	}
	painter->restore();
}

void CrossSectionWindowGraphicsView::drawLines(QPainter* painter, const QMatrix& matrix)
{
	// @todo add function to change color
	painter->save();
	painter->setPen(Qt::black);

	auto cs = chartWindow()->currentCrossSection();
	auto points = cs->mappedPoints();
	for (int i = 0; i < points.size() - 1; ++i) {
		auto p1 = matrix.map(toP(points.at(i)));
		auto p2 = matrix.map(toP(points.at(i + 1)));
		painter->drawLine(p1, p2);
	}

	painter->restore();
}

void CrossSectionWindowGraphicsView::drawCircles(QPainter* painter, const QMatrix& matrix)
{
	painter->save();
	painter->setBrush(QBrush(m_crossSectionWindowDisplaySetting.XSColor));
	painter->setPen(Qt::black);

	auto cs = chartWindow()->currentCrossSection();
	auto points = cs->mappedPoints();
	for (int i = 0; i < points.size(); ++i) {
		auto p = matrix.map(toP(points.at(i)));
		painter->drawEllipse(p, ellipseR, ellipseR);
	}

	painter->restore();
}

CrossSectionWindow* CrossSectionWindowGraphicsView::chartWindow() const
{
	return dynamic_cast<CrossSectionWindow*> (ChartGraphicsView::chartWindow());
}

QRectF CrossSectionWindowGraphicsView::getRegion()
{
	double xmin, xmax, ymin, ymax;
	auto cs = chartWindow()->currentCrossSection();
	auto points = cs->mappedPoints();
	for (int i = 0; i < points.size(); ++i) {
		auto p = points.at(i);
		if (i == 0 || p.x() < xmin) {xmin = p.x();}
		if (i == 0 || p.x() > xmax) {xmax = p.x();}
		if (i == 0 || p.y() < ymin) {ymin = p.y();}
		if (i == 0 || p.y() > ymax) {ymax = p.y();}
	}
	return QRectF(xmin, ymin, (xmax - xmin), (ymax - ymin));
}
