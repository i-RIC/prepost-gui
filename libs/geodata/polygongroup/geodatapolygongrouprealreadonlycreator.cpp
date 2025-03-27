#include "geodatapolygongrouprealreadonlycreator.h"

GeoDataPolygonGroupRealReadOnlyCreator::GeoDataPolygonGroupRealReadOnlyCreator() :
	GeoDataPolygonGroupReadOnlyCreatorT<double, vtkDoubleArray>("realPolygonGroupReadOnly")
{}
