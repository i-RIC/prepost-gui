#ifndef POLYGONGEOREFERENCEVIEWHELPER_H
#define POLYGONGEOREFERENCEVIEWHELPER_H

#include "georeferenceviewhelper.h"

class PolygonGeoreferenceViewHelper : public GeoreferenceViewHelper
{
public:
	PolygonGeoreferenceViewHelper(const QPolygonF* polygon, GeoreferenceView* view);
	PolygonGeoreferenceViewHelper(const QPolygonF* polygon, GeoreferenceView* view, const QPen& pen);
	~PolygonGeoreferenceViewHelper();

	void paintView(QPainter* painter, const QTransform& transform, const QRectF& rect) const override;

private:
	const QPolygonF* m_polygon;
};

#endif // POLYGONGEOREFERENCEVIEWHELPER_H
