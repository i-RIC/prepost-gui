#include "polygonutil.h"

#include <QPolygonF>

std::vector<QPointF> PolygonUtil::toStdVector(const QPolygonF& polygon)
{
	std::vector<QPointF> ret;
	for (QPointF p : polygon) {
		ret.push_back(p);
	}

	return ret;
}

QPolygonF PolygonUtil::fromStdVector(const std::vector<QPointF>& points)
{
	QPolygonF ret;
	for (QPointF p : points) {
		ret.push_back(p);
	}

	return ret;
}


PolygonUtil::PolygonUtil()
{}
