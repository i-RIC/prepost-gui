#include "geodatapolylinegroupintegercreator.h"

GeoDataPolyLineGroupIntegerCreator::GeoDataPolyLineGroupIntegerCreator() :
	GeoDataPolyLineGroupIntegerCreator {"integerPolyLineGroup"}
{}

GeoDataPolyLineGroupIntegerCreator::GeoDataPolyLineGroupIntegerCreator(const QString& typeName) :
	GeoDataPolyLineGroupCreatorT<int, vtkIntArray> {typeName}
{}
