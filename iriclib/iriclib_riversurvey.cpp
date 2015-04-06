#include <fstream>
#include <stdlib.h>
#include "iriclib_riversurvey.h"
#include "iriclib_bstream.h"

using namespace std;
using namespace iRICLib;

RiverPathPoint::RiverPathPoint()
{
	nextPoint = 0;
	gridSkip = 0;
}

RiverPathPoint::~RiverPathPoint()
{
	if (nextPoint != 0){
		delete nextPoint;
	}
}

InputBStream& operator >> (InputBStream& stream, std::vector<double>& vec)
{
	int size;
	double val;
	stream >> size;
	vec.clear();
	for (int i = 0; i < size; ++i){
		stream >> val;
		vec.push_back(val);
	}
	return stream;
}

InputBStream& operator >> (InputBStream& stream, RiverPathPoint& p)
{
	int size;
	stream >> p.name;
	p.nameReal = atof(p.name.c_str());
	stream >> p.positionX >> p.positionY;
	stream >> p.directionX >> p.directionY;
	stream >> p.leftShift;
	stream >> size;
	p.altitudes.clear();
	for (unsigned int i = 0; i < size; ++i){
		Altitude alt;
		stream >> alt.position >> alt.height >> alt.active;
		p.altitudes.push_back(alt);
	}
	stream >> p.fixedPointLSet;
	if (p.fixedPointLSet != 0){
		stream >> p.directionLX >> p.directionLY >> p.fixedPointLIndex;
	}
	stream >> p.fixedPointRSet;
	if (p.fixedPointRSet != 0){
		stream >> p.directionRX >> p.directionRY >> p.fixedPointRIndex;
	}
	stream >> p.gridSkip;

	stream >> p.centerToLeftCtrlPoints;
	stream >> p.centerToRightCtrlPoints;
	stream >> p.centerLineCtrlPoints;
	stream >> p.leftBankCtrlPoints;
	stream >> p.rightBankCtrlPoints;

	stream >> p.wseSpecified;
	stream >> p.waterSurfaceElevation;

	return stream;
}

OutputBStream& operator << (OutputBStream& stream, const std::vector<double>& vec)
{
	stream << static_cast<int>(vec.size());
	for (unsigned int i = 0; i < vec.size(); ++i){
		stream << vec.at(i);
	}
	return stream;
}

OutputBStream& operator << (OutputBStream& stream, const RiverPathPoint& p)
{
	stream << p.name;
	stream << p.positionX << p.positionY;
	stream << p.directionX << p.directionY;
	stream << p.leftShift;
	stream << static_cast<int>(p.altitudes.size());
	for (unsigned int i = 0; i < p.altitudes.size(); ++i){
		Altitude alt = p.altitudes[i];
		stream << alt.position << alt.height << alt.active;
	}
	stream << p.fixedPointLSet;
	if (p.fixedPointLSet != 0){
		stream << p.directionLX << p.directionLY << p.fixedPointLIndex;
	}
	stream << p.fixedPointRSet;
	if (p.fixedPointRSet != 0){
		stream << p.directionRX << p.directionRY << p.fixedPointRIndex;
	}
	stream << p.gridSkip;

	stream << p.centerToLeftCtrlPoints;
	stream << p.centerToRightCtrlPoints;
	stream << p.centerLineCtrlPoints;
	stream << p.leftBankCtrlPoints;
	stream << p.rightBankCtrlPoints;

	stream << p.wseSpecified;
	stream << p.waterSurfaceElevation;

	return stream;
}

RiverSurvey::RiverSurvey()
{
	firstPoint = 0;
}
RiverSurvey::~RiverSurvey()
{
	if (firstPoint != 0){
		delete firstPoint;
	}
}

int RiverSurvey::load(const char* filename)
{
	ifstream istream(filename, ios::in | ios::binary);
	if (! istream){
		// open error
		return -1;
	}
	InputBStream str(istream);
	int size;
	points.clear();

	str >> size;
	RiverPathPoint* p;
	RiverPathPoint* prevP;
	for (int i = 0; i < size; ++i){
		p = new RiverPathPoint();
		str >> *p;
		if (i == 0){
			firstPoint = p;
		} else {
			prevP->nextPoint = p;
		}
		points.push_back(p);
		prevP = p;
	}
	istream.close();
	return 0;
}

int RiverSurvey::save(const char* filename)
{
	ofstream ostream(filename, ios::out | ios::binary);
	if (! ostream){
		// open error
		return -1;
	}
	OutputBStream str(ostream);
	points.clear();
	RiverPathPoint* p;
	if (firstPoint != 0){
		points.push_back(firstPoint);
		p = firstPoint;
		while (p->nextPoint != 0){
			p = p->nextPoint;
			points.push_back(p);
		}
	}

	str << static_cast<int>(points.size());
	for (unsigned int i = 0; i < points.size(); ++i){
		RiverPathPoint* p = points[i];
		str << *p;
	}
	ostream.close();
	return 0;
}
