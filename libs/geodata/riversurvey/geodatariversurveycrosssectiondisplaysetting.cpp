#include "geodatariversurveycrosssectiondisplaysetting.h"

GeoDataRiverSurveyCrossSectionDisplaySetting::GeoDataRiverSurveyCrossSectionDisplaySetting() :
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
	distanceMarkersFont {"Helvetica", 10},
	lbBankMarkersColor {160, 160, 50},
	lbBankMarkersFont {"Helvetica", 10}
{}
