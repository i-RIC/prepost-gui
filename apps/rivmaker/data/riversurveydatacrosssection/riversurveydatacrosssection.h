#ifndef RIVERSURVEYDATACROSSSECTION_H
#define RIVERSURVEYDATACROSSSECTION_H

class QVector3D;

#include <vector>

class CrossSection;

class RiverSurveyDataCrossSection
{
public:
	RiverSurveyDataCrossSection(CrossSection* cs);
	~RiverSurveyDataCrossSection();

	CrossSection* crossSection() const;

	double waterSurfaceElevation() const;
	void setWaterSurfaceElevation(double e);

	const std::vector<QVector3D*>& points() const;
	std::vector<QVector3D*>& points();

private:
	double m_waterSurfaceElevation;
	CrossSection* m_crossSection;
	std::vector<QVector3D*> m_points;
};

#endif // RIVERSURVEYDATACROSSSECTION_H
