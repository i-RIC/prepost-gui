#include "geodatapolygongroupintegercreator.h"

GeoDataPolygonGroupIntegerCreator::GeoDataPolygonGroupIntegerCreator() :
	GeoDataPolygonGroupCreatorT<int, vtkIntArray> {"integerPolygonGroup"}
{}
