#ifndef GEODATARIVERSURVEYCROSSSECTIONDISPLAYSETTING_H
#define GEODATARIVERSURVEYCROSSSECTIONDISPLAYSETTING_H

#include <QColor>
#include <QFont>

class GeoDataRiverSurveyCrossSectionDisplaySetting
{
public:
	enum class BackgroundGridType {
		Lines,
		Dots
	};

	enum class StatusBarXType {
		DistanceFromLeftBank,
		DistanceFromCenter
	};

	GeoDataRiverSurveyCrossSectionDisplaySetting();

	BackgroundGridType bgGridType;
	QColor bgGridColor;
	bool bgHScaleAuto;
	double bgHScaleInterval;
	double bgHSubScaleInterval;

	bool bgVScaleAuto;
	double bgVScaleInterval;
	double bgVSubScaleInterval;

	QColor aspectRatioColor;
	QFont aspectRatioFont;

	QColor distanceMarkersColor;
	QFont distanceMarkersFont;

	QColor lbBankMarkersColor;
	QFont lbBankMarkersFont;

	QColor odnStartColor;
	QColor odnMiddleColor;
	QColor odnLowColor;
	QFont odnNbFont;

	StatusBarXType statusBarXType;
};

#endif // GEODATARIVERSURVEYCROSSSECTIONDISPLAYSETTING_H
