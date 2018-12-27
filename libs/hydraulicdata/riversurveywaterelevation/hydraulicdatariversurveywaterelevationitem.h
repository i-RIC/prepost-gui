#ifndef HYDRAULICDATARIVERSURVEYWATERELEVATIONITEM_H
#define HYDRAULICDATARIVERSURVEYWATERELEVATIONITEM_H

#include "hd_riversurveywaterelevation_global.h"

#include <QString>

class HD_RIVERSURVEYWATERELEVATION_EXPORT HydraulicDataRiverSurveyWaterElevationItem
{
public:
	HydraulicDataRiverSurveyWaterElevationItem(const QString& name, bool specified, double value);

	const QString& name() const;
	void setName(const QString& name);

	bool isSpecified() const;
	void setSpecified(bool specified);

	double value() const;
	void setValue(double v);

private:
	QString m_name;
	bool m_isSpecified;
	double m_value;
};

#endif // HYDRAULICDATARIVERSURVEYWATERELEVATIONITEM_H
