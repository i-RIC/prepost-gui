#ifndef RECTGEOREFERENCEVIEWHELPER_H
#define RECTGEOREFERENCEVIEWHELPER_H

#include "georeferenceviewhelper.h"

class RectGeoreferenceViewHelper : public GeoreferenceViewHelper
{
public:
	RectGeoreferenceViewHelper(const QRectF* rect, GeoreferenceView* view);
	RectGeoreferenceViewHelper(const QRectF* rect, GeoreferenceView* view, const QPen& pen);
	~RectGeoreferenceViewHelper();

	void paintView(QPainter* painter, const QTransform& transform, const QRectF& rect) const override;

private:
	const QRectF* m_rect;
};

#endif // RECTGEOREFERENCEVIEWHELPER_H
