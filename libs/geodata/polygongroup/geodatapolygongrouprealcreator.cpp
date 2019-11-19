#include "geodatapolygongrouprealcreator.h"

GeoDataPolygonGroupRealCreator::GeoDataPolygonGroupRealCreator() :
	GeoDataPolygonGroupCreatorT<double, vtkDoubleArray> {"realPolygonGroup"}
{}
