#ifndef IRICLIB_RIVERSURVEY_H
#define IRICLIB_RIVERSURVEY_H

#include "iriclib_global.h"
#include <string>
#include <vector>

namespace iRICLib {
class IRICLIBDLL Altitude {
public:
	double position;
	double height;
	int active;
};

class IRICLIBDLL RiverPathPoint {
public:
	RiverPathPoint();
	~RiverPathPoint();

	double positionX;
	double positionY;
	double directionX;
	double directionY;

	std::string name;
	double nameReal;

	double leftShift;

	std::vector<Altitude> altitudes;

	int fixedPointLSet;
	double directionLX;
	double directionLY;
	int fixedPointLIndex;

	int fixedPointRSet;
	double directionRX;
	double directionRY;
	int fixedPointRIndex;

	int gridSkip;

	std::vector<double> centerToLeftCtrlPoints;
	std::vector<double> centerToRightCtrlPoints;
	std::vector<double> centerLineCtrlPoints;
	std::vector<double> leftBankCtrlPoints;
	std::vector<double> rightBankCtrlPoints;

	int wseSpecified;
	double waterSurfaceElevation;

	RiverPathPoint* nextPoint;
};

class IRICLIBDLL RiverSurvey {
public:
	RiverSurvey();
	~RiverSurvey();
	RiverPathPoint* firstPoint;
	std::vector<RiverPathPoint*> points;
	int load(const char* filename);
	int save(const char* filename);
};
}
#endif // IRICLIB_RIVERSURVEY_H
