#include "geodatapolylinerealcreator.h"

GeoDataPolyLineRealCreator::GeoDataPolyLineRealCreator() :
	GeoDataPolyLineCreatorT<double, vtkDoubleArray> {"realPolyLine"}
{}
