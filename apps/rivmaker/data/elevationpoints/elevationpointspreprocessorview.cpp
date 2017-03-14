#include "elevationpoints.h"
#include "elevationpointspreprocessorview.h"
#include "elevationpointspreprocessorviewhelper.h"

#include <QRectF>
#include <QVector3D>

ElevationPointsPreprocessorView::ElevationPointsPreprocessorView(Model* model, ElevationPoints* item) :
	DataItemView {model, item}
{
	viewHelpers().push_back(new ElevationPointsPreProcessorViewHelper(this));
}

ElevationPointsPreprocessorView::~ElevationPointsPreprocessorView()
{}

QRectF ElevationPointsPreprocessorView::doBoundingBox() const
{
	auto points = dynamic_cast<ElevationPoints*> (item());
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
