#include "leftbankhwmpreprocessorviewhelper.h"
#include "../points/pointsgraphicssetting.h"

#include <QColor>

LeftBankHWMPreProcessorViewHelper::LeftBankHWMPreProcessorViewHelper(DataItemView* v) :
	PointsPreProcessorViewHelper {v}
{}

void LeftBankHWMPreProcessorViewHelper::draw(QPainter* painter) const
{
	auto setting = PointsGraphicsSetting::waterElevationPointsSetting;
	drawRects(setting.size(), Qt::blue, setting.transparency, painter);
}
