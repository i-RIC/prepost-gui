#ifndef PROJECT_H
#define PROJECT_H

#include <QObject>

#include <vector>

class BaseLine;
class CrossSection;
class CrossSections;
class ElevationPoints;
class RiverSurveyData;
class RiverSurveyDataDummy;
class RootDataItem;
class WaterSurfaceElevationPoints;

class QWidget;

class Project : public QObject
{
	Q_OBJECT

public:
	Project();
	~Project();

	RootDataItem* rootDataItem() const;

	const ElevationPoints& elevationPoints() const;
	ElevationPoints& elevationPoints();

	const WaterSurfaceElevationPoints& waterSurfaceElevationPoints() const;
	WaterSurfaceElevationPoints& waterSurfaceElevationPoints();

	const BaseLine& baseLine() const;
	BaseLine& baseLine();

	const CrossSections& crossSections() const;
	CrossSections& crossSections();

	bool hasRiverSurveyData() const;
	RiverSurveyData* riverSurveyData() const;

	const RiverSurveyDataDummy& riverSurveyDataDummy() const;
	RiverSurveyDataDummy& riverSurveyDataDummy();

	const QPointF& offset() const;
	QPointF& offset();

	void calcCrossSectionElevations();
	bool sortCrossSectionsIfPossible();

	bool checkIfReadyToOpenVerticalCrossSectionWindow(QWidget* w) const;
	bool checkIfReadyToCreateRiverSurveyData(QWidget* w) const;

	void importElevationPoints(QWidget* w);
	void importWaterSurfaceElevationPoints(QWidget* w);
	void importBaseLine(QWidget* w);
	void importCrossSections(QWidget* w);

	void exportElevationPoints(QWidget* w);
	void exportWaterSurfaceElevationPoints(QWidget* w);
	void exportBaseLine(QWidget* w);
	void exportCrossSections(QWidget* w);
	void exportRiverSurveyData(QWidget* w);

	void createRiverSurveyData();
	void deleteRiverSurveyData();

public slots:
	void emitUpdated();

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
