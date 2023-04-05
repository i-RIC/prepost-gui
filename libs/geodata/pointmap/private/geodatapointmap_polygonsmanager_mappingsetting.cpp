#include "geodatapointmap_polygonsmanager_mappingsetting.h"

namespace {

const double DEFAULT_RADIUS = 10;

} // namespace

GeoDataPointmap::PolygonsManager::MappingSetting::MappingSetting() :
	type {Type::WeightedAverage},
	radius {DEFAULT_RADIUS},
	exponent {1}
{}

QVariant GeoDataPointmap::PolygonsManager::MappingSetting::toVariant()
{
	QList<QVariant> values;
	values.push_back(static_cast<int> (type));
	values.push_back(radius);
	values.push_back(exponent);

	return values;
}

void GeoDataPointmap::PolygonsManager::MappingSetting::fromVariant(const QVariant value)
{
	auto list = value.toList();
	if (list.size() != 3) {return;}

	type = static_cast<Type> (list.at(0).toInt());
	radius = list.at(1).toDouble();
	exponent = list.at(2).toDouble();
}
