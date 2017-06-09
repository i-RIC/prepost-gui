#include "arbitraryhwmpreprocessorviewhelper.h"
#include "../points/pointsgraphicssetting.h"

#include <QColor>

ArbitraryHWMPreProcessorViewHelper::ArbitraryHWMPreProcessorViewHelper(DataItemView* v) :
	PointsPreProcessorViewHelper {v}
{}

void ArbitraryHWMPreProcessorViewHelper::draw(QPainter* painter) const
{
	auto setting = PointsGraphicsSetting::waterElevationPointsSetting;
	drawRects(setting.size(), Qt::gray, setting.transparency, painter);
}
