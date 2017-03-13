#ifndef PROJECT_H
#define PROJECT_H

#include <QObject>

#include <vector>

class BaseLine;
class CrossSection;
class ElevationPoints;
class RiverSurveyData;
class WaterSurfaceElevationPoints;

class QWidget;

class Project : public QObject
{
	Q_OBJECT

public:
	Project();
	~Project();

	const ElevationPoints& elevationPoints() const;
	ElevationPoints& elevationPoints();

	const WaterSurfaceElevationPoints& waterSurfaceElevationPoints() const;
	WaterSurfaceElevationPoints& waterSurfaceElevationPoints();

	const BaseLine& baseLine() const;
	BaseLine baseLine();

	const std::vector<CrossSection*>& crossSections() const;
	std::vector<CrossSection*>& crossSections();

	bool checkIfReadyToOpenVerticalCrossSectionWindow(QWidget* w) const;
	bool checkIfReadyToCreateRiverSurveyData(QWidget* w) const;

	bool hasRiverSurveyData() const;
	RiverSurveyData* riverSurveyData() const;

public slots:
	void importElevationPoints();
	void importWaterSurfaceElevationPoints();
	void importBaseLine();
	void importCrossSections();

	void exportElevationPoints();
	void exportWaterSurfaceElevationPoints();
	void exportBaseLine();
	void exportCrossSections();
	void exportRiverSurveyData();

	void createRiverSurveyData();
	void deleteRiverSurveyData();

signals:
	void updated();

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/project_impl.h"
#endif // _DEBUG

#endif // PROJECT_H
