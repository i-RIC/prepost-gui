#include "geodatapolygonrealcreator.h"

GeoDataPolygonRealCreator::GeoDataPolygonRealCreator() :
	GeoDataPolygonCreatorT<double, vtkDoubleArray>()
{
	m_typeName = "realPolygon";
}
