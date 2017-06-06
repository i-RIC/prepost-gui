#include "rightbankhwmpreprocessorviewhelper.h"
#include "../points/pointsgraphicssetting.h"

#include <QColor>

RightBankHWMPreProcessorViewHelper::RightBankHWMPreProcessorViewHelper(DataItemView* v) :
	PointsPreProcessorViewHelper {v}
{}

void RightBankHWMPreProcessorViewHelper::draw(QPainter* painter) const
{
	auto setting = PointsGraphicsSetting::waterElevationPointsSetting;
	drawRects(STD_SIZE, Qt::red, setting.transparency, painter);
}
