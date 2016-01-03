#include "geodatapolygonrealcreator.h"

GeoDataPolygonRealCreator::GeoDataPolygonRealCreator() :
	GeoDataPolygonCreatorT<double, vtkDoubleArray> {"realPolygon"}
{}
