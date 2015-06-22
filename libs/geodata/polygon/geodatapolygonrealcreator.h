#ifndef GEODATAPOLYGONREALCREATOR_H
#define GEODATAPOLYGONREALCREATOR_H

#include "geodatapolygoncreatort.h"
#include <vtkDoubleArray.h>

class GeoDataPolygonRealCreator : public GeoDataPolygonCreatorT<double, vtkDoubleArray>
{

public:
	GeoDataPolygonRealCreator()
		: GeoDataPolygonCreatorT<double, vtkDoubleArray>() {
		m_typeName = "realPolygon";
	}
};

#endif // GEODATAPOLYGONREALCREATOR_H
