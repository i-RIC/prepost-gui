#include "geodatapolylineintegercreator.h"

GeoDataPolyLineIntegerCreator::GeoDataPolyLineIntegerCreator() :
	GeoDataPolyLineCreatorT<int, vtkIntArray> {"integerPolyLine"}
{}
