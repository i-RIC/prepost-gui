#ifndef GEODATAPOINTMAPREALTEXTIMPORTER_VALUES
#define GEODATAPOINTMAPREALTEXTIMPORTER_VALUES

#include "../geodatapointmaprealtextimporter.h"

class GeoDataPointmapRealTextImporter::Values
{
public:
	Values() : x {0}, y {0}, value {0} {}

	double x;
	double y;
	double value;
};

#endif // GEODATAPOINTMAPREALTEXTIMPORTER_VALUES
