#include "geodatapointrealcreator.h"

GeoDataPointRealCreator::GeoDataPointRealCreator() :
	GeoDataPointCreatorT<double, vtkDoubleArray> {"realPoint"}
{}
