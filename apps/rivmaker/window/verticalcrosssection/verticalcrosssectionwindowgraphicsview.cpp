#include "verticalcrosssectionwindow.h"
#include "verticalcrosssectionwindowgraphicsview.h"
#include "../../data/baseline/baseline.h"
#include "../../data/base/view.h"
#include "../../geom/geometrypoint.h"
#include "../../data/crosssection/crosssection.h"
#include "../../data/points/points.h"
#include "../../data/points/pointspreprocessorviewhelper.h"
#include "../../data/project/project.h"
#include "../../data/watersurfaceelevationpoints/watersurfaceelevationpoints.h"

#include <QFontMetrics>
#include <QMatrix>
#include <QMouseEvent>
#include <QPainter>

namespace {

const int LABEL_OFFSET = 8;
const int XS_NAME_X_OFFSET = 5;
const int XS_NAME_Y_OFFSET = 10;

typedef void (*drawfunc)(const QPointF&, int, const QColor&, int, QPainter*);

void updateRegion(const BaseLine& bline, const Points& points, double* xmin, double* xmax, double* ymin, double* ymax, bool* first)
{
	bool l_internal;
	for (auto p : points.points()) {
		double pos = bline.calcPosition(p->x(), p->y(), &l_internal);
		if (*first || pos < *xmin) {*xmin = pos;}
		if (*first || pos > *xmax) {*xmax = pos;}
		if (*first || p->z() < *ymin) {*ymin = p->z();}
		if (*first || p->z() > *ymax) {*ymax = p->z();}

		*first = false;
	}
}

void drawName(const QPointF& pos, const QString& name, QPainter* painter)
{
	int size = painter->font().pointSize();

	if (name.isNull()) {return;}
	QPointF p = pos + QPointF(LABEL_OFFSET, size * 0.5);

	painter->save();
	painter->setPen(Qt::black);
	painter->drawText(p, name);
	painter->restore();
}

void drawErrorBar(const QPointF& p1, const QPointF& p2, int width, QPainter* painter)
{
	painter->drawLine(p1, p2);
	painter->drawLine(QPointF(p1.x() - width, p1.y()), QPointF(p1.x() + width, p1.y()));
	painter->drawLine(QPointF(p2.x() - width, p2.y()), QPointF(p2.x() + width, p2.y()));

}

void drawMarkers(const BaseLine& bline, const Points& points, const QColor& color, drawfunc func, QPainter* painter, const QMatrix& matrix)
{
	bool l_internal;
	for (auto p : points.points()) {
		double pos = bline.calcPosition(p->x(), p->y(), &l_internal);
		auto p2 = matrix.map(QPointF(pos, p->z()));
		(*func)(p2, PointsPreProcessorViewHelper::STD_SIZE, color, 0, painter);

		if (p->error() != 0) {
			painter->save();
			painter->setPen(QPen(color, 1));
			auto ep1 = matrix.map(QPointF(pos, p->z() - p->error()));
			auto ep2 = matrix.map(QPointF(pos, p->z() + p->error()));
			drawErrorBar(ep1, ep2, 3, painter);
			painter->restore();
		}
		drawName(p2, p->name(), painter);
	}
}

} // namespace

VerticalCrossSectionWindowGraphicsView::VerticalCrossSectionWindowGraphicsView(QWidget* parent) :
	ChartGraphicsView(parent),
	m_drawn {false}
{}

void VerticalCrossSectionWindowGraphicsView::paintEvent(QPaintEvent* /*event*/)
{
	QPainter painter(this);
	draw(&painter);
}

void VerticalCrossSectionWindowGraphicsView::draw(QPainter* painter)
{
	QRect r = rect();
	auto matrix = getMatrix(r);

	paintBackground(painter);
	drawScales(painter, matrix);
	drawAspectRatio(painter);

	if (! chartWindow()->project()->baseLine().isDefined()) {return;}

	drawLeftHWMLine(painter, matrix);
	drawRightHWMLine(painter, matrix);

	drawArbitrary(painter, matrix);
	drawBenchmark(painter, matrix);
	drawReferenceMark(painter, matrix);
	drawHub(painter, matrix);
	drawLeftHWMs(painter, matrix);
	drawRightHWMs(painter, matrix);
	drawStreamGage(painter, matrix);
	drawCrossSectionLines(painter, matrix);
	drawElevationLine(painter, matrix);
}

void VerticalCrossSectionWindowGraphicsView::updateView()
{
	auto p = chartWindow()->project();

	if (! m_drawn && p->baseLine().isDefined()) {
		cameraFit();
	} else {
		update();
	}
	m_drawn = p->baseLine().isDefined();
}

void VerticalCrossSectionWindowGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
	if (viewMouseEventMode() != vmeNormal) {
		mouseMoveEventViewChangeHandler(event);
		return;
	}
	if (m_mouseEventMode == meMove) {
		auto m = getMatrix(rect()).inverted();
		auto newPos = m.map(QPointF(event->pos()));

		auto p = chartWindow()->project();
		CrossSection* cs = p->crossSections().crossSectionVector().at(m_editTargetCrossSection);
		cs->setWaterElevation(newPos.y());

		p->emitUpdated();
	} else {
		updateMouseEventMode(event);
		updateMouseCursor();
	}
}

void VerticalCrossSectionWindowGraphicsView::mousePressEvent(QMouseEvent* event)
{
	mousePressEventViewChangeHandler(event);
	if (viewMouseEventMode() != vmeNormal) {return;}

	if (m_mouseEventMode == meMovePrepare) {
		m_mouseEventMode = meMove;
		updateMouseCursor();
	}
}

void VerticalCrossSectionWindowGraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
	mouseReleaseEventViewChangeHandler(event);
	updateMouseEventMode(event);
	updateMouseCursor();
}

void VerticalCrossSectionWindowGraphicsView::drawArbitrary(QPainter* painter, const QMatrix& matrix)
{
	if (! chartWindow()->showArbitrary()) {return;}

	auto p = chartWindow()->project();
	drawMarkers(p->baseLine(), p->waterSurfaceElevationPoints().arbitraryHWM(), Qt::gray, PointsPreProcessorViewHelper::drawRect, painter, matrix);
}

void VerticalCrossSectionWindowGraphicsView::drawBenchmark(QPainter* painter, const QMatrix& matrix)
{
	if (! chartWindow()->showBenchmark()) {return;}

	auto p = chartWindow()->project();
	drawMarkers(p->baseLine(), p->waterSurfaceElevationPoints().benchmark(), Qt::black, PointsPreProcessorViewHelper::drawReverseTriangle, painter, matrix);
}

void VerticalCrossSectionWindowGraphicsView::drawReferenceMark(QPainter* painter, const QMatrix& matrix)
{
	if (! chartWindow()->showReferenceMark()) {return;}

	auto p = chartWindow()->project();
	drawMarkers(p->baseLine(), p->waterSurfaceElevationPoints().referenceMark(), Qt::black, PointsPreProcessorViewHelper::drawTriangle, painter, matrix);
}

void VerticalCrossSectionWindowGraphicsView::drawHub(QPainter* painter, const QMatrix& matrix)
{
	if (! chartWindow()->showHub()) {return;}

	auto p = chartWindow()->project();
	drawMarkers(p->baseLine(), p->waterSurfaceElevationPoints().hub(), Qt::gray, PointsPreProcessorViewHelper::drawDiamond, painter, matrix);
}

void VerticalCrossSectionWindowGraphicsView::drawLeftHWMs(QPainter* painter, const QMatrix& matrix)
{
	if (! chartWindow()->showLeftHWMs()) {return;}

	auto p = chartWindow()->project();
	drawMarkers(p->baseLine(), p->waterSurfaceElevationPoints().leftBankHWM(), Qt::blue, PointsPreProcessorViewHelper::drawRect, painter, matrix);
}

void VerticalCrossSectionWindowGraphicsView::drawRightHWMs(QPainter* painter, const QMatrix& matrix)
{
	if (! chartWindow()->showRightHWMs()) {return;}

	auto p = chartWindow()->project();
	drawMarkers(p->baseLine(), p->waterSurfaceElevationPoints().rightBankHWM(), Qt::red, PointsPreProcessorViewHelper::drawRect, painter, matrix);
}

void VerticalCrossSectionWindowGraphicsView::drawStreamGage(QPainter* painter, const QMatrix& matrix)
{
	if (! chartWindow()->showStreamGage()) {return;}

	auto p = chartWindow()->project();
	drawMarkers(p->baseLine(), p->waterSurfaceElevationPoints().streamGage(), Qt::gray, PointsPreProcessorViewHelper::drawRect, painter, matrix);
}

void VerticalCrossSectionWindowGraphicsView::drawLeftHWMLine(QPainter* painter, const QMatrix& matrix)
{
	if (! chartWindow()->showLeftHWMLine()) {return;}

}

void VerticalCrossSectionWindowGraphicsView::drawRightHWMLine(QPainter* painter, const QMatrix& matrix)
{
	if (! chartWindow()->showRightHWMLine()) {return;}

}

void VerticalCrossSectionWindowGraphicsView::drawCrossSectionLines(QPainter* painter, const QMatrix& matrix)
{
	if (! chartWindow()->showXSLine()) {return;}

	painter->save();

	auto p = chartWindow()->project();
	const auto& baseLine = p->baseLine();
	auto csVec = p->crossSections().crossSectionVector();

	bool crosses;
	double x, y, pos;

	QPen pen(Qt::black);
	pen.setWidth(2);
	pen.setStyle(Qt::DashLine);
	painter->setPen(pen);
	QFont f("Hervetica", 12);
	f.setBold(true);
	painter->setFont(f);

	QFontMetrics metrics(painter->font());

	for (CrossSection* cs : csVec) {
		baseLine.getCrossingPoint(cs, &crosses, &x, &y, &pos);
		if (! crosses) {continue;}

		auto point = matrix.map(QPointF(pos, 0));
		painter->drawLine(QPointF(point.x(), 0), QPointF(point.x(), height()));
		auto rect = metrics.boundingRect(cs->name());

		painter->save();
		painter->translate(point.x() - XS_NAME_X_OFFSET, XS_NAME_Y_OFFSET + rect.width());
		painter->rotate(-90);
		painter->drawText(0, 0, cs->name());

		painter->restore();
	}

	painter->restore();
}

void VerticalCrossSectionWindowGraphicsView::drawElevationLine(QPainter* painter, const QMatrix& matrix)
{
	if (! chartWindow()->showWSELine()) {return;}

	painter->save();

	auto p = chartWindow()->project();
	const auto& baseLine = p->baseLine();
	auto csVec = p->crossSections().crossSectionVector();

	bool crosses;
	double x, y, pos;

	QPen pen(Qt::black);
	pen.setWidth(1);
	painter->setPen(pen);

	bool first = true;
	QPointF prevPoint;

	for (CrossSection* cs : csVec) {
		baseLine.getCrossingPoint(cs, &crosses, &x, &y, &pos);
		if (! crosses) {continue;}

		auto point = matrix.map(QPointF(pos, cs->waterElevation()));
		if (! first) {
			painter->drawLine(prevPoint, point);
		}
		prevPoint = point;
		first = false;
	}

	painter->restore();
}

VerticalCrossSectionWindow* VerticalCrossSectionWindowGraphicsView::chartWindow() const
{
	return dynamic_cast<VerticalCrossSectionWindow*> (ChartGraphicsView::chartWindow());
}

QRectF VerticalCrossSectionWindowGraphicsView::getRegion()
{
	bool first = true;
	double xmin, xmax, ymin, ymax;

	auto p = chartWindow()->project();
	const auto& baseLine = p->baseLine();
	if (! baseLine.isDefined()){
		return QRectF(0, 0, 1, 1);
	}

	const auto& wse = p->waterSurfaceElevationPoints();
	auto w = chartWindow();

	if (w->showArbitrary()) {
		updateRegion(baseLine, wse.arbitraryHWM(), &xmin, &xmax, &ymin, &ymax, &first);
	}
	if (w->showBenchmark()) {
		updateRegion(baseLine, wse.benchmark(), &xmin, &xmax, &ymin, &ymax, &first);
	}
	if (w->showReferenceMark()) {
		updateRegion(baseLine, wse.referenceMark(), &xmin, &xmax, &ymin, &ymax, &first);
	}
	if (w->showHub()) {
		updateRegion(baseLine, wse.hub(), &xmin, &xmax, &ymin, &ymax, &first);
	}
	if (w->showLeftHWMs() || w->showLeftHWMLine()) {
		updateRegion(baseLine, wse.leftBankHWM(), &xmin, &xmax, &ymin, &ymax, &first);
	}
	if (w->showRightHWMs() || w->showRightHWMLine()) {
		updateRegion(baseLine, wse.rightBankHWM(), &xmin, &xmax, &ymin, &ymax, &first);
	}
	if (w->showStreamGage()) {
		updateRegion(baseLine, wse.streamGage(), &xmin, &xmax, &ymin, &ymax, &first);
	}

	if (first) {
		return QRectF(0, 0, 1, 1);
	} else {
		return QRectF(xmin, ymin, xmax - xmin, ymax - ymin);
	}
}

void VerticalCrossSectionWindowGraphicsView::updateMouseEventMode(QMouseEvent* event)
{
	auto p = chartWindow()->project();
	const auto& baseLine = p->baseLine();
	auto csVec = p->crossSections().crossSectionVector();

	bool crosses;
	double x, y, pos;

	m_mouseEventMode = meNormal;

	int idx = 0;
	auto matrix = getMatrix(rect());

	for (CrossSection* cs : csVec) {
		baseLine.getCrossingPoint(cs, &crosses, &x, &y, &pos);
		if (crosses) {
			auto point = matrix.map(QPointF(pos, cs->waterElevation()));

			if (View::isNear(event->pos(), point)) {
				m_mouseEventMode = meMovePrepare;
				m_editTargetCrossSection = idx;
			}
		}
		++ idx;
	}
}

void VerticalCrossSectionWindowGraphicsView::updateMouseCursor()
{
	if (m_mouseEventMode == meMove) {
		setCursor(Qt::ClosedHandCursor);
	} else if (m_mouseEventMode == meMovePrepare) {
		setCursor(Qt::OpenHandCursor);
	} else {
		setCursor(Qt::ArrowCursor);
	}
}
