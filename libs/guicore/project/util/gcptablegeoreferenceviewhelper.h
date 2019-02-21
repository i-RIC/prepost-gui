#ifndef GCPTABLEGEOREFERENCEVIEWHELPER_H
#define GCPTABLEGEOREFERENCEVIEWHELPER_H

#include "georeferenceviewhelper.h"

#include <QPointF>

class GcpTableRow;

class GcpTableGeoreferenceViewHelper : public GeoreferenceViewHelper
{
public:
	GcpTableGeoreferenceViewHelper(const std::vector<GcpTableRow>* points, GeoreferenceView* view);
	GcpTableGeoreferenceViewHelper(const std::vector<GcpTableRow>* points, GeoreferenceView* view, const QPen& pen);
	GcpTableGeoreferenceViewHelper(const std::vector<GcpTableRow>* points, GeoreferenceView* view, const QBrush& brush);
	GcpTableGeoreferenceViewHelper(const std::vector<GcpTableRow>* points, GeoreferenceView* view, const QPen& pen, const QBrush& brush);
	~GcpTableGeoreferenceViewHelper();

	void paintView(QPainter* painter, const QTransform& transform, const QRectF& rect) const override;

private:
	const std::vector<GcpTableRow>* m_points;
};

#endif // GCPTABLEGEOREFERENCEVIEWHELPER_H
