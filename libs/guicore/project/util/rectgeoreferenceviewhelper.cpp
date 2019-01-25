#include "rectgeoreferenceviewhelper.h"
#include "../private/georeferenceview_imageinfo.h"
#include "../georeferenceview.h"

#include <guibase/qpainterpenbrushchanger.h>

#include <QPainter>
#include <QPolygonF>

RectGeoreferenceViewHelper::RectGeoreferenceViewHelper(const QRectF* rect, GeoreferenceView* view) :
	GeoreferenceViewHelper {view},
	m_rect {rect}
{}

RectGeoreferenceViewHelper::RectGeoreferenceViewHelper(const QRectF* rect, GeoreferenceView* view, const QPen& pen) :
	GeoreferenceViewHelper {view, pen},
	m_rect {rect}
{}

RectGeoreferenceViewHelper::~RectGeoreferenceViewHelper()
{}

void RectGeoreferenceViewHelper::paintView(QPainter* painter, const QTransform&, const QRectF&) const
{
	// Avoid meaningless drawing when no line exists.
	if (! m_rect->isValid()) {return;}

	GeoreferenceView* v = view();

	QPolygonF pol {};

	auto xMin = m_rect->left();
	auto yMin = m_rect->top();
	auto xMax = m_rect->left() + m_rect->width();
	auto yMax = m_rect->top()  + m_rect->height();

	pol.push_back(v->conv(QPointF {xMin, yMin}));
	pol.push_back(v->conv(QPointF {xMin, yMax}));
	pol.push_back(v->conv(QPointF {xMax, yMax}));
	pol.push_back(v->conv(QPointF {xMax, yMin}));

	QPainterPenBrushChanger penBrushChanger(painter, pen(), brush());
	painter->drawPolygon(pol);
}
