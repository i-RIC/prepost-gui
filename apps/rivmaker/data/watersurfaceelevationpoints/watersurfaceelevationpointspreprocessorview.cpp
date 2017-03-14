#include "watersurfaceelevationpoints.h"
#include "watersurfaceelevationpointspreprocessorview.h"
#include "watersurfaceelevationpointspreprocessorviewhelper.h"

#include <QRectF>
#include <QVector3D>

namespace {

void updateMinMax(const std::vector<QVector3D*>& points, double* xmin, double* xmax, double* ymin, double* ymax, bool* first)
{
	for (QVector3D* v : points) {
		if (*first || v->x() < *xmin) {*xmin = v->x();}
		if (*first || v->x() > *xmax) {*xmax = v->x();}
		if (*first || v->y() < *ymin) {*ymin = v->y();}
		if (*first || v->y() > *ymax) {*ymax = v->y();}

		*first = false;
	}
}

} // namespace

WaterSurfaceElevationPointsPreProcessorView::WaterSurfaceElevationPointsPreProcessorView(Model *model, WaterSurfaceElevationPoints *item) :
	DataItemView {model, item}
{
	viewHelpers().push_back(new WaterSurfaceElevationPointsPreProcessorViewHelper(this));
}

WaterSurfaceElevationPointsPreProcessorView::~WaterSurfaceElevationPointsPreProcessorView()
{}

QRectF WaterSurfaceElevationPointsPreProcessorView::doBoundingBox() const
{
	auto points = dynamic_cast<WaterSurfaceElevationPoints*> (item());

	if (points->leftBankPoints().size() == 0 && points->rightBankPoints().size() == 0 && points->arbitraryPoints().size() == 0) {
		return QRectF();
	}

	bool first = true;
	double xmin, xmax, ymin, ymax;


	updateMinMax(points->leftBankPoints(), &xmin, &xmax, &ymin, &ymax, &first);
	updateMinMax(points->rightBankPoints(), &xmin, &xmax, &ymin, &ymax, &first);
	updateMinMax(points->arbitraryPoints(), &xmin, &xmax, &ymin, &ymax, &first);

	return QRectF(QPointF(xmin, ymin), QPointF(xmax, ymax));
}
