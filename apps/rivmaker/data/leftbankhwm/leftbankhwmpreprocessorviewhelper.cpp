#include "leftbankhwmpreprocessorviewhelper.h"

#include <QColor>

LeftBankHWMPreProcessorViewHelper::LeftBankHWMPreProcessorViewHelper(DataItemView* v) :
	PointsPreProcessorViewHelper {v}
{}

void LeftBankHWMPreProcessorViewHelper::draw(QPainter* painter) const
{
	drawRects(7, Qt::blue, painter);
}
