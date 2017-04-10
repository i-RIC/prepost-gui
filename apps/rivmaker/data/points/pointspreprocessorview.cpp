#include "pointspreprocessorview.h"

#include <QRectF>
#include <QVector3D>

PointsPreProcessorView::PointsPreProcessorView(Model* model, Points* item) :
	DataItemView {model, item}
{}

PointsPreProcessorView::~PointsPreProcessorView()
{}

QRectF PointsPreProcessorView::doBoundingBox() const
{
	auto points = dynamic_cast<Points*> (item());
	if (points->points().size() == 0) {
		return QRectF();
	}

	bool first = true;
	double xmin, xmax, ymin, ymax;

	for (QVector3D* v : points->points()) {
		if (first || v->x() < xmin) {xmin = v->x();}
		if (first || v->x() > xmax) {xmax = v->x();}
		if (first || v->y() < ymin) {ymin = v->y();}
		if (first || v->y() > ymax) {ymax = v->y();}

		first = false;
	}
	return QRectF(QPointF(xmin, ymin), QPointF(xmax, ymax));
}
