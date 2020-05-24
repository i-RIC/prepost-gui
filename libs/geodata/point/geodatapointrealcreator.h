#ifndef GEODATAPOINTREALCREATOR_H
#define GEODATAPOINTREALCREATOR_H

#include "geodatapointcreatort.h"
#include <vtkDoubleArray.h>

class GD_POINT_EXPORT GeoDataPointRealCreator : public GeoDataPointCreatorT<double, vtkDoubleArray>
{
public:
	GeoDataPointRealCreator();
};

#endif // GEODATAPOINTREALCREATOR_H
