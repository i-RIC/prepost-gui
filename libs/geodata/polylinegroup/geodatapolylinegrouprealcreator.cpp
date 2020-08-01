#include "geodatapolylinegrouprealcreator.h"

GeoDataPolyLineGroupRealCreator::GeoDataPolyLineGroupRealCreator() :
	GeoDataPolyLineGroupCreatorT<double, vtkDoubleArray> {"realPolyLineGroup"}
{}
