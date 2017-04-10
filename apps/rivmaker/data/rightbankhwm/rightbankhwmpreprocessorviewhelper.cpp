#include "rightbankhwmpreprocessorviewhelper.h"

#include <QColor>

RightBankHWMPreProcessorViewHelper::RightBankHWMPreProcessorViewHelper(DataItemView* v) :
	PointsPreProcessorViewHelper {v}
{}

void RightBankHWMPreProcessorViewHelper::draw(QPainter* painter) const
{
	drawRects(7, Qt::red, painter);
}
