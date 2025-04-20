#include "geodatapolygongroupintegercreator.h"

GeoDataPolygonGroupIntegerCreator::GeoDataPolygonGroupIntegerCreator() :
	GeoDataPolygonGroupIntegerCreator {"integerPolygonGroup"}
{}

GeoDataPolygonGroupIntegerCreator::GeoDataPolygonGroupIntegerCreator(const QString& typeName) :
	GeoDataPolygonGroupCreatorT<int, vtkIntArray> {typeName}
{}
