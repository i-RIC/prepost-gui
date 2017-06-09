#include "benchmarkpreprocessorviewhelper.h"
#include "../points/pointsgraphicssetting.h"

#include <QColor>

BenchmarkPreProcessorViewHelper::BenchmarkPreProcessorViewHelper(DataItemView* v) :
	PointsPreProcessorViewHelper {v}
{}

void BenchmarkPreProcessorViewHelper::draw(QPainter* painter) const
{
	auto setting = PointsGraphicsSetting::waterElevationPointsSetting;
	drawReverseTriangles(setting.size(), Qt::black, setting.transparency, painter);
}
