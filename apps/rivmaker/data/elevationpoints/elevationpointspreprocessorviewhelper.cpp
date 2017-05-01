#include "elevationpoints.h"
#include "elevationpointspreprocessorviewhelper.h"
#include "../points/pointsgraphicssetting.h"

#include <QColor>

ElevationPointsPreProcessorViewHelper::ElevationPointsPreProcessorViewHelper(DataItemView* v) :
	PointsPreProcessorViewHelper {v}
{}

void ElevationPointsPreProcessorViewHelper::draw(QPainter* painter) const
{
	auto setting = PointsGraphicsSetting::elevationPointsSetting;
	drawCircles(setting.size(), Qt::black, setting.transparency, painter);
}
