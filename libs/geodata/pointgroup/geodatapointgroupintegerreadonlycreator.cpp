#include "geodatapointgroupintegerreadonlycreator.h"

GeoDataPointGroupIntegerReadOnlyCreator::GeoDataPointGroupIntegerReadOnlyCreator() :
	GeoDataPointGroupReadOnlyCreatorT<int, vtkIntArray> {"integerPointGroupReadOnly"}
{}
