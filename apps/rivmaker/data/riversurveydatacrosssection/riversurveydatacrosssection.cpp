#include "riversurveydatacrosssection.h"

RiverSurveyDataCrossSection::RiverSurveyDataCrossSection(CrossSection* cs) :
	m_waterSurfaceElevation {0}
{}

RiverSurveyDataCrossSection::~RiverSurveyDataCrossSection()
{}

CrossSection* RiverSurveyDataCrossSection::crossSection() const
{
	return m_crossSection;
}

double RiverSurveyDataCrossSection::waterSurfaceElevation() const
{
	return m_waterSurfaceElevation;
}

void RiverSurveyDataCrossSection::setWaterSurfaceElevation(double e)
{
	m_waterSurfaceElevation = e;
}

const std::vector<QVector3D*>& RiverSurveyDataCrossSection::points() const
{
	return m_points;
}

std::vector<QVector3D*>& RiverSurveyDataCrossSection::points()
{
	return m_points;
}
