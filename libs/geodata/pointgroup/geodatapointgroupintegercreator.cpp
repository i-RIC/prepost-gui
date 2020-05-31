#include "geodatapointgroupintegercreator.h"

GeoDataPointGroupIntegerCreator::GeoDataPointGroupIntegerCreator() :
	GeoDataPointGroupCreatorT<int, vtkIntArray> {"integerPointGroup"}
{}
