#include "geodatapolylinegrouprealcreator.h"

GeoDataPolyLineGroupRealCreator::GeoDataPolyLineGroupRealCreator() :
	GeoDataPolyLineGroupRealCreator {"realPolyLineGroup"}
{}

GeoDataPolyLineGroupRealCreator::GeoDataPolyLineGroupRealCreator(const QString& typeName) :
	GeoDataPolyLineGroupCreatorT<double, vtkDoubleArray> {typeName}
{}
