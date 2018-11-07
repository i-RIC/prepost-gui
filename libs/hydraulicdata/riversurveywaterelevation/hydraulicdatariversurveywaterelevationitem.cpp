#include "hydraulicdatariversurveywaterelevationitem.h"

HydraulicDataRiverSurveyWaterElevationItem::HydraulicDataRiverSurveyWaterElevationItem(const QString& name, bool specified, double value) :
	m_name {name},
	m_isSpecified {specified},
	m_value {value}
{}

const QString& HydraulicDataRiverSurveyWaterElevationItem::name() const
{
	return m_name;
}

void HydraulicDataRiverSurveyWaterElevationItem::setName(const QString& name)
{
	m_name = name;
}

bool HydraulicDataRiverSurveyWaterElevationItem::isSpecified() const
{
	return m_isSpecified;
}

void HydraulicDataRiverSurveyWaterElevationItem::setSpecified(bool specified)
{
	m_isSpecified = specified;
}

double HydraulicDataRiverSurveyWaterElevationItem::value() const
{
	return m_value;
}

void HydraulicDataRiverSurveyWaterElevationItem::setValue(double v)
{
	m_value = v;
}
