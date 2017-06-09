#include "streamgagepreprocessorviewhelper.h"
#include "../points/pointsgraphicssetting.h"

#include <QColor>

StreamGagePreProcessorViewHelper::StreamGagePreProcessorViewHelper(DataItemView* v) :
	PointsPreProcessorViewHelper {v}
{}

void StreamGagePreProcessorViewHelper::draw(QPainter* painter) const
{
	auto setting = PointsGraphicsSetting::waterElevationPointsSetting;
	drawRects(setting.size(), Qt::gray, setting.transparency, painter);
}
