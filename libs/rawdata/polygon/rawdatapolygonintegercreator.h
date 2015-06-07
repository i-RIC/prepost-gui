#ifndef RAWDATAPOLYGONINTEGERCREATOR_H
#define RAWDATAPOLYGONINTEGERCREATOR_H

#include "rawdatapolygoncreatort.h"
#include <vtkIntArray.h>

class RawDataPolygonIntegerCreator : public RawDataPolygonCreatorT<int, vtkIntArray>
{

public:
	RawDataPolygonIntegerCreator()
		: RawDataPolygonCreatorT<int, vtkIntArray>() {
		m_typeName = "integerPolygon";
	}
};

#endif // RAWDATAPOLYGONINTEGERCREATOR_H
