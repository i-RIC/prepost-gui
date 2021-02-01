#include "qpointfcompare.h"

#include <QPointF>

QPointFCompare::QPointFCompare()
{}

bool QPointFCompare::operator()(const QPointF& a, const QPointF& b)const
{
	if (a.x() == b.x()) {
		return a.y() < b.y();
	} else {
		return a.x() < b.x();
	}
}
