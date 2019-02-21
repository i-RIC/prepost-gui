#include "pointsgeoreferenceviewhelper.h"

#include <guibase/qpainterpenbrushchanger.h>

#include <QPainter>

#include <vector>

namespace {
const static int RADIUS = 3;
}

PointsGeoreferenceViewHelper::PointsGeoreferenceViewHelper(const std::vector<QPointF>* points, GeoreferenceView* view) :
	PointsGeoreferenceViewHelper {points, view, QPen {}, QBrush {}}
{}

PointsGeoreferenceViewHelper::PointsGeoreferenceViewHelper(const std::vector<QPointF>* points, GeoreferenceView* view, const QPen& pen) :
	PointsGeoreferenceViewHelper {points, view, pen, QBrush {}}
{}

PointsGeoreferenceViewHelper::PointsGeoreferenceViewHelper(const std::vector<QPointF>* points, GeoreferenceView* view, const QBrush& brush) :
	PointsGeoreferenceViewHelper {points, view, QPen {}, brush}
{}

PointsGeoreferenceViewHelper::PointsGeoreferenceViewHelper(const std::vector<QPointF>* points, GeoreferenceView* view, const QPen& pen, const QBrush& brush) :
	GeoreferenceViewHelper {view, pen, brush},
	m_points {points},
	m_radius {RADIUS}
{}

void PointsGeoreferenceViewHelper::paintView(QPainter* painter, const QTransform&, const QRectF&) const
{
	// Avoid meaningless drawing when no line exists.
	if (m_points->empty()) {return;}

	QPainterPenBrushChanger penBrushChanger(painter, pen(), brush());

	for (auto p : *m_points) {
		QPointF pf = view()->conv(p);
		painter->drawEllipse(pf, m_radius, m_radius);
	}
}

PointsGeoreferenceViewHelper* PointsGeoreferenceViewHelper::setRadius(int radius)
{
	m_radius = radius;
	return this;
}
