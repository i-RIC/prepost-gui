#include "elevationpoints.h"
#include "elevationpointspreprocessorviewhelper.h"

#include <QColor>

ElevationPointsPreProcessorViewHelper::ElevationPointsPreProcessorViewHelper(DataItemView* v) :
	PointsPreProcessorViewHelper {v}
{}

void ElevationPointsPreProcessorViewHelper::draw(QPainter* painter) const
{
	drawCircles(7, Qt::black, painter);
}
