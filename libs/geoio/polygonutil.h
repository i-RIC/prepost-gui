#ifndef POLYGONUTIL_H
#define POLYGONUTIL_H

#include <vector>

class QPointF;
class QPolygonF;

class PolygonUtil
{
public:
	static std::vector<QPointF> toStdVector(const QPolygonF& polygon);
	static QPolygonF fromStdVector(const std::vector<QPointF>& points);

private:
	PolygonUtil();
};

#endif // POLYGONUTIL_H
