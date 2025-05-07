#include "geodatapolygongrouprealcreator.h"

GeoDataPolygonGroupRealCreator::GeoDataPolygonGroupRealCreator() :
	GeoDataPolygonGroupRealCreator {"realPolygonGroup"}
{}

GeoDataPolygonGroupRealCreator::GeoDataPolygonGroupRealCreator(const QString& typeName) :
	GeoDataPolygonGroupCreatorT<double, vtkDoubleArray> {typeName}
{}
