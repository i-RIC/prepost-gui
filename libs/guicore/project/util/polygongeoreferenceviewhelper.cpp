#include "polygongeoreferenceviewhelper.h"
#include "../private/georeferenceview_imageinfo.h"
#include "../georeferenceview.h"

#include <guibase/qpainterpenbrushchanger.h>

#include <QPainter>
#include <QPolygonF>

PolygonGeoreferenceViewHelper::PolygonGeoreferenceViewHelper(const QPolygonF* polygon, GeoreferenceView* view) :
	GeoreferenceViewHelper {view},
	m_polygon {polygon}
{}

PolygonGeoreferenceViewHelper::PolygonGeoreferenceViewHelper(const QPolygonF* polygon, GeoreferenceView* view, const QPen& pen) :
	GeoreferenceViewHelper {view, pen},
	m_polygon {polygon}
{}

PolygonGeoreferenceViewHelper::~PolygonGeoreferenceViewHelper()
{}

void PolygonGeoreferenceViewHelper::paintView(QPainter* painter, const QTransform&, const QRectF&) const
{
	// Avoid meaningless drawing when no line exists.
	if (m_polygon->size() < 2) {return;}

	GeoreferenceView* v = view();

	QPolygonF pol;
	for (const auto& p : *m_polygon) {
		QPointF pf(v->conv(QPointF(p.x(), p.y())));
		pol.push_back(pf);
	}

	QPainterPenBrushChanger penBrushChanger(painter, pen(), brush());
	painter->drawPolygon(pol);
}
