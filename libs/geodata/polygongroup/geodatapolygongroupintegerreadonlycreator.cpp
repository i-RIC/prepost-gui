#include "geodatapolygongroupintegerreadonlycreator.h"

GeoDataPolygonGroupIntegerReadOnlyCreator::GeoDataPolygonGroupIntegerReadOnlyCreator() :
	GeoDataPolygonGroupReadOnlyCreatorT<int, vtkIntArray>("integerPolygonGroupReadOnly")
{}
