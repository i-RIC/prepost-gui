#include <fstream>
#include "iriclib_pointmap.h"
#include "iriclib_bstream.h"

using namespace std;
using namespace iRICLib;

PointMapTriangle::PointMapTriangle()
{
	index1 = 0;
	index2 = 0;
	index3 = 0;
}

InputBStream& operator >> (InputBStream& stream, PointMapTriangle& tri)
{
	stream >> tri.index1;
	stream >> tri.index2;
	stream >> tri.index3;

	return stream;
}

OutputBStream& operator << (OutputBStream& stream, const PointMapTriangle& tri)
{
	stream << tri.index1;
	stream << tri.index2;
	stream << tri.index3;

	return stream;
}

InputBStream& operator >> (InputBStream& stream, PointMapBreakline& bl)
{
	bl.indices.clear();
	int length;
	stream >> length;
	bl.indices.reserve(length);

	for (int i = 0; i < length; ++i){
		int index;
		stream >> index;
		bl.indices.push_back(index);
	}

	return stream;
}

OutputBStream& operator << (OutputBStream& stream, const PointMapBreakline& bl)
{
	stream << static_cast<int>(bl.indices.size());
	for (int i = 0; i < bl.indices.size(); ++i){
		stream << bl.indices.at(i);
	}

	return stream;
}

PointMap::PointMap()
{
	pointCount = 0;
	valueType = vtReal;
}
PointMap::~PointMap()
{
	clear();
}

int PointMap::load(const char* filename)
{
	ifstream istream(filename, ios::in | ios::binary);
	if (! istream){
		// open error
		return -1;
	}
	clear();

	InputBStream str(istream);
	int vt;
	str >> vt;
	valueType = static_cast<ValueType>(vt);

	str >> pointCount;
	x.reserve(pointCount);
	y.reserve(pointCount);
	if (valueType == vtReal){
		realValue.reserve(pointCount);
	} else if (valueType == vtInt){
		intValue.reserve(pointCount);
	}

	for (int i = 0; i < pointCount; ++i){
		double tmpx;
		str >> tmpx;
		x.push_back(tmpx);
	}
	for (int i = 0; i < pointCount; ++i){
		double tmpy;
		str >> tmpy;
		y.push_back(tmpy);
	}
	if (valueType == vtReal){
		for (int i = 0; i < pointCount; ++i){
			double tmpv;
			str >> tmpv;
			realValue.push_back(tmpv);
		}
	} else if (valueType == vtInt){
		for (int i = 0; i < pointCount; ++i){
			int tmpv;
			str >> tmpv;
			intValue.push_back(tmpv);
		}
	}
	int triCount;
	str >> triCount;
	triangles.reserve(triCount);
	for (int i = 0; i < triCount; ++i){
		PointMapTriangle tri;
		str >> tri;
		triangles.push_back(tri);
	}
	int blCount;
	str >> blCount;
	breaklines.reserve(blCount);
	for (int i = 0; i < blCount; ++i){
		PointMapBreakline bl;
		str >> bl;
		breaklines.push_back(bl);
	}
	istream.close();
	return 0;
}

int PointMap::save(const char *filename)
{
	ofstream ostream(filename, ios::out | ios::binary);
	if (! ostream){
		// open error
		return -1;
	}
	OutputBStream str(ostream);
	str << static_cast<int>(valueType);

	pointCount = x.size();
	str << pointCount;

	for (int i = 0; i < pointCount; ++i){
		str << x.at(i);
	}
	for (int i = 0; i < pointCount; ++i){
		str << y.at(i);
	}
	if (valueType == vtReal){
		for (int i = 0; i < pointCount; ++i){
			str << realValue.at(i);
		}
	} else if (valueType == vtInt){
		for (int i = 0; i < pointCount; ++i){
			str << intValue.at(i);
		}
	}
	int triCount = triangles.size();
	str << triCount;
	for (int i = 0; i < triCount; ++i){
		str << triangles.at(i);
	}
	int blCount = breaklines.size();
	str << blCount;
	for (int i = 0; i < blCount; ++i){
		str << breaklines.at(i);
	}
	ostream.close();
	return 0;
}

void PointMap::clear()
{
	pointCount = 0;
	x.clear();
	y.clear();
	realValue.clear();
	intValue.clear();
	triangles.clear();
	breaklines.clear();
}
