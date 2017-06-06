#include "pointspreprocessorviewnameshelper.h"

PointsPreProcessorViewNamesHelper::PointsPreProcessorViewNamesHelper(DataItemView* v) :
	PointsPreProcessorViewHelper {v}
{}

void PointsPreProcessorViewNamesHelper::draw(QPainter* painter) const
{
	drawNames(painter);
}
