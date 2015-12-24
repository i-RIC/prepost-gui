#include "geodatapolygonintegercreator.h"

GeoDataPolygonIntegerCreator::GeoDataPolygonIntegerCreator() :
	GeoDataPolygonCreatorT<int, vtkIntArray>()
{
	m_typeName = "integerPolygon";
}
