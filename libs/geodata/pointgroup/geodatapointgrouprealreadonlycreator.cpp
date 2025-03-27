#include "geodatapointgrouprealreadonlycreator.h"

GeoDataPointGroupRealReadOnlyCreator::GeoDataPointGroupRealReadOnlyCreator() :
	GeoDataPointGroupReadOnlyCreatorT<double, vtkDoubleArray> {"realPointGroupReadOnly"}
{}
