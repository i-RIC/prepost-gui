#include "elevationpointspreprocessorview.h"
#include "elevationpointspreprocessorviewhelper.h"

ElevationPointsPreprocessorView::ElevationPointsPreprocessorView(Model* model, ElevationPoints* item) :
	PointsPreProcessorView {model, item}
{
	viewHelpers().push_back(new ElevationPointsPreProcessorViewHelper(this));
}

ElevationPointsPreprocessorView::~ElevationPointsPreprocessorView()
{}

void ElevationPointsPreprocessorView::drawMarker(const QPointF&, QPainter*) const
{}
