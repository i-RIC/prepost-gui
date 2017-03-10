#ifndef PROJECT_H
#define PROJECT_H

class BaseLine;
class CrossSection;
class ElevationPoints;
class RiverSurveyData;
class WaterSurfaceElevationPoints;

class QWidget;

class Project
{
public:
	Project();
	~Project();

	const ElevationPoints& elevationPoints() const;
	ElevationPoints& elevationPoints();

	const WaterSurfaceElevationPoints& waterSurfaceElevationPoints() const;
	WaterSurfaceElevationPoints& waterSurfaceElevationPoints();

	const BaseLine& baseLine() const;
	BaseLine baseLine();

	bool checkIfReadyToCreateRiverSurveyData(QWidget* w) const;

	void createRiverSurveyData();

	bool hasRiverSurveyData() const;
	RiverSurveyData* riverSurveyData() const;
	void deleteRiverSurveyData();

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/project_impl.h"
#endif // _DEBUG

#endif // PROJECT_H
