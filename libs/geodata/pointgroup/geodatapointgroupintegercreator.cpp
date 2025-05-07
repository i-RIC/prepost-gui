#include "geodatapointgroupintegercreator.h"

GeoDataPointGroupIntegerCreator::GeoDataPointGroupIntegerCreator() :
	GeoDataPointGroupIntegerCreator {"integerPointGroup"}
{}

GeoDataPointGroupIntegerCreator::GeoDataPointGroupIntegerCreator(const QString& typeName) :
	GeoDataPointGroupCreatorT<int, vtkIntArray> {typeName}
{}
