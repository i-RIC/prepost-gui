#ifndef HYDRAULICDATARIVERSURVEYWATERELEVATION_IMPL_H
#define HYDRAULICDATARIVERSURVEYWATERELEVATION_IMPL_H

#include "../hydraulicdatariversurveywaterelevation.h"
#include "../hydraulicdatariversurveywaterelevationitem.h"

#include <QColor>
#include <QString>

#include <map>

class HydraulicDataRiverSurveyWaterElevation::Impl
{
public:
	std::map<QString, HydraulicDataRiverSurveyWaterElevationItem> m_items;
	QColor m_color;
};

#endif // HYDRAULICDATARIVERSURVEYWATERELEVATION_IMPL_H
