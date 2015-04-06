#ifndef IRICLIB_POINTMAP_H
#define IRICLIB_POINTMAP_H

#include "iriclib_global.h"
#include <vector>

namespace iRICLib {
class IRICLIBDLL PointMapTriangle {
public:
	PointMapTriangle();
	int index1;
	int index2;
	int index3;
};

class IRICLIBDLL PointMapBreakline {
public:
	std::vector<int> indices;
};

class IRICLIBDLL PointMap {
public:
	enum ValueType {
		vtInt  = 0,
		vtReal = 1
	};

	PointMap();
	~PointMap();

	int pointCount;
	ValueType valueType;
	std::vector<double> x;
	std::vector<double> y;
	std::vector<double> realValue;
	std::vector<int> intValue;

	std::vector<PointMapTriangle> triangles;
	std::vector<PointMapBreakline> breaklines;

	int load(const char* filename);
	int save(const char* filename);
	void clear();
};
}

#endif // IRICLIB_POINTMAP_H
