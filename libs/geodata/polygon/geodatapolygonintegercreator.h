#ifndef GEODATAPOLYGONINTEGERCREATOR_H
#define GEODATAPOLYGONINTEGERCREATOR_H

#include "geodatapolygoncreatort.h"
#include <vtkIntArray.h>

class GeoDataPolygonIntegerCreator : public GeoDataPolygonCreatorT<int, vtkIntArray>
{

public:
	GeoDataPolygonIntegerCreator()
		: GeoDataPolygonCreatorT<int, vtkIntArray>() {
		m_typeName = "integerPolygon";
	}
};

#endif // GEODATAPOLYGONINTEGERCREATOR_H
