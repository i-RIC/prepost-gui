#include "geodatapolygonintegercreator.h"

GeoDataPolygonIntegerCreator::GeoDataPolygonIntegerCreator() :
	GeoDataPolygonCreatorT<int, vtkIntArray> {"integerPolygon"}
{}
