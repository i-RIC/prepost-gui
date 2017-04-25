#include <fstream>
#include "iriclib_polygon.h"
#include "iriclib_bstream.h"

using namespace std;
using namespace iRICLib;

InternalPolygon::InternalPolygon()
{
	x = 0;
	y = 0;
	pointCount = 0;
}

InternalPolygon::~InternalPolygon()
{
	if (x != 0){ delete x; }
	if (y != 0){ delete y; }
}

InputBStream& operator >> (InputBStream& stream, InternalPolygon& pol)
{
	stream >> pol.pointCount;
	pol.x = new double[pol.pointCount];
	pol.y = new double[pol.pointCount];

	for (int i = 0; i < pol.pointCount; ++i){
		stream >> *(pol.x + i);
	}
	for (int i = 0; i < pol.pointCount; ++i){
		stream >> *(pol.y + i);
	}
	return stream;
}

OutputBStream& operator << (OutputBStream& stream, const InternalPolygon& pol)
{
	stream << pol.pointCount;
	for (int i = 0; i < pol.pointCount; ++i){
		stream << *(pol.x + i);
	}
	for (int i = 0; i < pol.pointCount; ++i){
		stream << *(pol.y + i);
	}
	return stream;
}


Polygon::Polygon()
{
	polygon = 0;
}

Polygon::~Polygon()
{
	clear();
}

int Polygon::load(const char* filename, bool noDimensions)
{
	ifstream istream(filename, ios::in | ios::binary);
	if (! istream){
		// open error
		return -1;
	}
	clear();

	InputBStream str(istream);
	int size;
	values.clear();
	double val;
	if (noDimensions){
		str >> val;
		values.push_back(val);
	} else {
		int count;
		str >> count;
		for (int i = 0; i < count; ++i){
			str >> val;
			values.push_back(val);
		}
	}
	polygon = new iRICLib::InternalPolygon();
	str >> *polygon;
	str >> size;

	holes.clear();
	for (int i = 0; i < size; ++i){
		InternalPolygon* hole = new InternalPolygon();
		str >> *hole;
		holes.push_back(hole);
	}
	istream.close();
	return 0;
}

int Polygon::save(const char *filename, bool noDimensions)
{
	ofstream ostream(filename, ios::out | ios::binary);
	if (! ostream){
		// open error
		return -1;
	}
	OutputBStream str(ostream);
	if (noDimensions){
		str << values[0];
	} else {
		int count = static_cast<int>(values.size());
		str << count;
		for (int i = 0; i < count; ++i){
			str << values[i];
		}
	}
	str << *polygon;
	str << static_cast<int>(holes.size());
	for (unsigned int i = 0; i < holes.size(); ++i){
		InternalPolygon* hole = holes[i];
		str << *hole;
	}
	ostream.close();
	return 0;
}

void Polygon::clear()
{
	if (polygon != 0){delete polygon;}
	for (unsigned int i = 0; i < holes.size(); ++i){
		delete holes[i];
	}
	holes.clear();
}
