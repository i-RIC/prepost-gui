#ifndef GEODATAPOINTMAP_POLYGONSMANAGER_MAPPINGSETTING_H
#define GEODATAPOINTMAP_POLYGONSMANAGER_MAPPINGSETTING_H

#include "geodatapointmap_polygonsmanager.h"

class GeoDataPointmap::PolygonsManager::MappingSetting
{
public:
	enum class Type {
		TIN,
		Maximum,
		Minimum,
		WeightedAverage
	};

	MappingSetting();

	QVariant toVariant();
	void fromVariant(const QVariant value);

	Type type;
	double radius;
	double exponent;
};

#endif // GEODATAPOINTMAP_POLYGONSMANAGER_MAPPINGSETTING_H
