#ifndef POINTSGEOREFERENCEVIEWHELPER_H
#define POINTSGEOREFERENCEVIEWHELPER_H

#include "georeferenceviewhelper.h"

class PointsGeoreferenceViewHelper : public GeoreferenceViewHelper
{
public:
	PointsGeoreferenceViewHelper(const std::vector<QPointF>* points, GeoreferenceView* view);
	PointsGeoreferenceViewHelper(const std::vector<QPointF>* points, GeoreferenceView* view, const QPen& pen);
	PointsGeoreferenceViewHelper(const std::vector<QPointF>* points, GeoreferenceView* view, const QBrush& brush);
	PointsGeoreferenceViewHelper(const std::vector<QPointF>* points, GeoreferenceView* view, const QPen& pen, const QBrush& brush);

	void paintView(QPainter* painter, const QTransform& transform, const QRectF& rect) const;

	PointsGeoreferenceViewHelper* setRadius(int radius);

private:
	const std::vector<QPointF>* m_points;
	int m_radius;
};

#endif // POINTSGEOREFERENCEVIEWHELPER_H
