#ifndef IRICLIB_POLYGON_H
#define IRICLIB_POLYGON_H

#include "iriclib_global.h"
#include <vector>

namespace iRICLib {
class IRICLIBDLL InternalPolygon {
public:
	InternalPolygon();
	~InternalPolygon();
	double* x;
	double* y;
	int pointCount;
};

class IRICLIBDLL Polygon {
public:
	Polygon();
	~Polygon();
	std::vector<double> values;
	InternalPolygon* polygon;
	std::vector<InternalPolygon*> holes;

	int load(const char* filename, bool noDimensions = false);
	int save(const char* filename, bool noDimensions = false);
	void clear();
};
}

#endif // IRICLIB_POLYGON_H
