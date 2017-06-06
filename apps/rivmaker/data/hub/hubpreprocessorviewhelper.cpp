#include "hubpreprocessorviewhelper.h"
#include "../points/pointsgraphicssetting.h"

#include <QColor>

HubPreProcessorViewHelper::HubPreProcessorViewHelper(DataItemView* v) :
	PointsPreProcessorViewHelper {v}
{}

void HubPreProcessorViewHelper::draw(QPainter* painter) const
{
	auto setting = PointsGraphicsSetting::waterElevationPointsSetting;
	drawDiamonds(STD_SIZE, Qt::gray, setting.transparency, painter);
}
