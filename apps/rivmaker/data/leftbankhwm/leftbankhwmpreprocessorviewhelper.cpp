#include "leftbankhwmpreprocessorviewhelper.h"
#include "../points/pointsgraphicssetting.h"

#include <QColor>

LeftBankHWMPreProcessorViewHelper::LeftBankHWMPreProcessorViewHelper(DataItemView* v) :
	PointsPreProcessorViewHelper {v}
{}

void LeftBankHWMPreProcessorViewHelper::draw(QPainter* painter) const
{
	auto setting = PointsGraphicsSetting::waterElevationPointsSetting;
	drawRects(STD_SIZE, Qt::blue, setting.transparency, painter);
}
