#include "points.h"
#include "pointspreprocessorviewhelper.h"
#include "../base/view.h"
#include "../../geom/geometrypoint.h"

#include <QColor>
#include <QPainter>
#include <QPointF>

PointsPreProcessorViewHelper::PointsPreProcessorViewHelper(DataItemView* v) :
	DataItemViewHelperI {v}
{}

void PointsPreProcessorViewHelper::drawCircles(int size, const QColor& color, int transparency, QPainter* painter) const
{
	painter->save();

	auto v = view();
	auto points = dynamic_cast<Points*> (dataItem());
	const auto& pvec = points->points();

	QColor c = color;
	c.setAlphaF(1.0 - transparency / 100.0);
	painter->setBrush(c);
	painter->setPen(Qt::NoPen);
	for (GeometryPoint* p : pvec) {
		QPointF p2 = v->conv(QPointF(p->x(), p->y()));

		QRectF rect(p2.x() - size * 0.5, p2.y() - size * 0.5, size, size);
		painter->drawEllipse(rect);
	}

	painter->restore();
}

void PointsPreProcessorViewHelper::drawRects(int size, const QColor& color, int transparency, QPainter* painter) const
{
	painter->save();

	auto v = view();
	auto points = dynamic_cast<Points*> (dataItem());
	const auto& pvec = points->points();

	QColor c = color;
	c.setAlphaF(1.0 - transparency / 100.0);
	for (GeometryPoint* p : pvec) {
		QPointF p2 = v->conv(QPointF(p->x(), p->y()));

		QRectF rect(p2.x() - size * 0.5, p2.y() - size * 0.5, size, size);
		painter->fillRect(rect, c);
	}

	painter->restore();
}
