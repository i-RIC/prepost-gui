#include "geodatapolylinegroupintegercreator.h"

GeoDataPolyLineGroupIntegerCreator::GeoDataPolyLineGroupIntegerCreator() :
	GeoDataPolyLineGroupCreatorT<int, vtkIntArray> {"integerPolyLineGroup"}
{}
