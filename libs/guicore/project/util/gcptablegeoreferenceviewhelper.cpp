#include "gcptablegeoreferenceviewhelper.h"
#include "../private/georeferenceview_imageinfo.h"
#include "../georeferenceview.h"
#include "../gcptablerow.h"

#include <guibase/qpainterpenbrushchanger.h>

#include <QPainter>

#include <vector>

namespace {
const static int RADIUS = 3;
}

GcpTableGeoreferenceViewHelper::GcpTableGeoreferenceViewHelper(const std::vector<GcpTableRow>* points, GeoreferenceView* view) :
	GcpTableGeoreferenceViewHelper {points, view, QPen {}, QBrush {}}
{}

GcpTableGeoreferenceViewHelper::GcpTableGeoreferenceViewHelper(const std::vector<GcpTableRow>* points, GeoreferenceView* view, const QPen& pen) :
	GcpTableGeoreferenceViewHelper {points, view, pen, QBrush {}}
{}

GcpTableGeoreferenceViewHelper::GcpTableGeoreferenceViewHelper(const std::vector<GcpTableRow>* points, GeoreferenceView* view, const QBrush& brush) :
	GcpTableGeoreferenceViewHelper {points, view, QPen {}, brush}
{}

GcpTableGeoreferenceViewHelper::GcpTableGeoreferenceViewHelper(const std::vector<GcpTableRow>* points, GeoreferenceView* view, const QPen& pen, const QBrush& brush) :
	GeoreferenceViewHelper {view, pen, brush},
	m_points {points}
{}

GcpTableGeoreferenceViewHelper::~GcpTableGeoreferenceViewHelper()
{}

void GcpTableGeoreferenceViewHelper::paintView(QPainter* painter, const QTransform&, const QRectF&) const
{
	// Avoid meaningless drawing when no line exists.
	if (m_points->empty()) {return;}

	QPainterPenBrushChanger penBrushChanger(painter, pen(), brush());

	for (auto p : *m_points) {
		QPointF pf = view()->conv(QPointF {p.sourceX, p.sourceY});
		painter->drawEllipse(pf, RADIUS, RADIUS);
	}
}
