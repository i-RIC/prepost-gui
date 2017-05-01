#ifndef PROJECT_H
#define PROJECT_H

#include <QObject>

#include <vector>

class BaseLine;
class CrossSection;
class CrossSections;
class ElevationPoints;
class RootDataItem;
class WaterSurfaceElevationPoints;

class QWidget;

class Project : public QObject
{
	Q_OBJECT

public:
	enum class MappingMethod {
		AllMapToNearestCrossSection,
		TIN,
		Template
	};

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

	const QPointF& offset() const;
	void setOffset(const QPointF& offset);

	void updatePointsAutoSize();

	MappingMethod mappingMethod() const;
	void setMappingMethod(MappingMethod method);

	double templateMappingResolution() const;
	void setTemplateMappingResolution(double resolution);

	double templateMappingStreamWiseLength() const;
	void setTemplateMappingStreamWiseLength(double len);

	double templateMappingCrossStreamWidth() const;
	void setTemplateMappingCrossStreamWidth(double width);

	int templateMappingNumberOfExpansions() const;
	void setTemplateMappingNumberOfExpansions(int num);

	double templateMappingWeightExponent() const;
	void setTemplateMappingWeightExponent(double exp);

	void calcCrossSectionElevations();
	void mapPointsToCrossSections();
	bool sortCrossSectionsIfPossible();

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
