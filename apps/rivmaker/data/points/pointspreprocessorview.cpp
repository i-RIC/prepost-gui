#include "pointspreprocessorview.h"
#include "../../geom/geometrypoint.h"

#include <QFont>
#include <QPainter>
#include <QRectF>

namespace {

int LEGEND_FONTSIZE = 10;

} // namespace

const int PointsPreProcessorView::LEGEND_WIDTH = 170;
const int PointsPreProcessorView::LEGEND_TOPMARGIN = 10;
const int PointsPreProcessorView::LEGEND_LINEHEIGHT = 14;

PointsPreProcessorView::PointsPreProcessorView(Model* model, Points* item) :
	DataItemView {model, item}
{}

PointsPreProcessorView::~PointsPreProcessorView()
{}

void PointsPreProcessorView::drawLegend(QPointF position, QPainter* painter) const
{
	drawMarker(position, painter);

	position.setX(position.x() + 10);
	position.setY(position.y() + LEGEND_FONTSIZE * 0.5);

	QFont f;
	f.setPointSize(LEGEND_FONTSIZE);

	painter->save();
	painter->setPen(Qt::black);
	painter->setFont(f);

	auto points = dynamic_cast<Points*> (item());
	painter->drawText(position, points->caption());

	painter->restore();
}

QRectF PointsPreProcessorView::doBoundingBox() const
{
	auto points = dynamic_cast<Points*> (item());
	if (points->points().size() == 0) {
		return QRectF();
	}

	bool first = true;
	double xmin, xmax, ymin, ymax;

	for (GeometryPoint* v : points->points()) {
		if (first || v->x() < xmin) {xmin = v->x();}
		if (first || v->x() > xmax) {xmax = v->x();}
		if (first || v->y() < ymin) {ymin = v->y();}
		if (first || v->y() > ymax) {ymax = v->y();}

		first = false;
	}
	return QRectF(QPointF(xmin, ymin), QPointF(xmax, ymax));
}
