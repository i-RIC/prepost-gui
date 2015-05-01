#ifndef RAWDATAPOLYGONREALCREATOR_H
#define RAWDATAPOLYGONREALCREATOR_H

#include "rawdatapolygoncreatort.h"
#include <vtkDoubleArray.h>

class RawDataPolygonRealCreator : public RawDataPolygonCreatorT<double, vtkDoubleArray>
{
public:
	RawDataPolygonRealCreator()
		: RawDataPolygonCreatorT<double, vtkDoubleArray>() {
		m_typeName = "realPolygon";
	}
};

#endif // RAWDATAPOLYGONREALCREATOR_H
