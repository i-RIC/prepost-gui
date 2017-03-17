#include "arbitraryhwmpreprocessorviewhelper.h"

#include <QColor>

ArbitraryHWMPreProcessorViewHelper::ArbitraryHWMPreProcessorViewHelper(DataItemView* v) :
	PointsPreProcessorViewHelper {v}
{}

void ArbitraryHWMPreProcessorViewHelper::draw(QPainter* painter) const
{
	drawRects(7, Qt::gray, painter);
}
