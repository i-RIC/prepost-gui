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

void PointsPreProcessorViewHelper::drawCircles(int size, const QColor& color, QPainter* painter) const
{
	painter->save();

	auto v = view();
	auto points = dynamic_cast<Points*> (dataItem());
	const auto& pvec = points->points();

	painter->setBrush(color);
	painter->setPen(Qt::NoPen);
	for (GeometryPoint* p : pvec) {
		QPointF p2 = v->conv(QPointF(p->x(), p->y()));

		QRectF rect(p2.x() - size * 0.5, p2.y() - size * 0.5, size, size);
		painter->drawEllipse(rect);
	}

	painter->restore();
}

void PointsPreProcessorViewHelper::drawRects(int size, const QColor& color, QPainter* painter) const
{
	painter->save();

	auto v = view();
	auto points = dynamic_cast<Points*> (dataItem());
	const auto& pvec = points->points();

	for (GeometryPoint* p : pvec) {
		QPointF p2 = v->conv(QPointF(p->x(), p->y()));

		QRectF rect(p2.x() - size * 0.5, p2.y() - size * 0.5, size, size);
		painter->fillRect(rect, color);
	}

	painter->restore();
}
