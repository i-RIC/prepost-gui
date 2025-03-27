#include "geodatapolylinegrouprealreadonlycreator.h"

GeoDataPolyLineGroupRealReadOnlyCreator::GeoDataPolyLineGroupRealReadOnlyCreator() :
	GeoDataPolyLineGroupReadOnlyCreatorT<double, vtkDoubleArray> {"realPolyLineGroupReadOnly"}
{}
