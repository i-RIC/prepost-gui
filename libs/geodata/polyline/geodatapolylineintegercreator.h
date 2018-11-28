#ifndef GEODATAPOLYLINEINTEGERCREATOR_H
#define GEODATAPOLYLINEINTEGERCREATOR_H

#include "geodatapolylinecreatort.h"
#include <vtkIntArray.h>

class GD_POLYLINE_EXPORT GeoDataPolyLineIntegerCreator : public GeoDataPolyLineCreatorT<int, vtkIntArray>
{
public:
	GeoDataPolyLineIntegerCreator();
};

#endif // GEODATAPOLYLINEINTEGERCREATOR_H
