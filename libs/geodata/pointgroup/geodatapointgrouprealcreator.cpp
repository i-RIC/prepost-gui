#include "geodatapointgrouprealcreator.h"

GeoDataPointGroupRealCreator::GeoDataPointGroupRealCreator() :
	GeoDataPointGroupCreatorT<double, vtkDoubleArray> {"realPointGroup"}
{}
