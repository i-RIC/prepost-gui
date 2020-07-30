#ifndef CHARTGRAPHICSVIEWDISPLAYSETTING_H
#define CHARTGRAPHICSVIEWDISPLAYSETTING_H

#include <QColor>
#include <QFont>

class ChartGraphicsViewDisplaySetting
{
public:
	enum class BackgroundGridType {
		Lines,
		Dots
	};

	ChartGraphicsViewDisplaySetting();

	QColor bgColor;

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
};

#endif // CHARTGRAPHICSVIEWDISPLAYSETTING_H
