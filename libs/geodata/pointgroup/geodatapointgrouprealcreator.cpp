#include "geodatapointgrouprealcreator.h"

GeoDataPointGroupRealCreator::GeoDataPointGroupRealCreator() :
	GeoDataPointGroupRealCreator {"realPointGroup"}
{}

GeoDataPointGroupRealCreator::GeoDataPointGroupRealCreator(const QString& typeName) :
	GeoDataPointGroupCreatorT<double, vtkDoubleArray> {typeName}
{}
