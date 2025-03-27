#include "geodatapolylinegroupintegerreadonlycreator.h"

GeoDataPolyLineGroupIntegerReadOnlyCreator::GeoDataPolyLineGroupIntegerReadOnlyCreator() :
	GeoDataPolyLineGroupReadOnlyCreatorT<int, vtkIntArray> {"integerPolyLineGroupReadOnly"}
{}
