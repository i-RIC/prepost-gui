#include "chartgraphicsviewdisplaysetting.h"

ChartGraphicsViewDisplaySetting::ChartGraphicsViewDisplaySetting() :
	bgColor {Qt::white},
	bgGridType {BackgroundGridType::Lines},
	bgGridColor {Qt::gray},
	bgHScaleAuto {true},
	bgHScaleInterval {10},
	bgHSubScaleInterval {2},
	bgVScaleAuto {true},
	bgVScaleInterval {10},
	bgVSubScaleInterval {2},
	aspectRatioColor {Qt::darkGray},
	aspectRatioFont {"Times", 10},
	distanceMarkersColor {160, 160, 50},
	distanceMarkersFont {"Helvetica", 10}
{}
