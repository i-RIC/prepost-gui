#include "v4structured15dgridwithcrosssectioncrosssection.h"

v4Structured15dGridWithCrossSectionCrossSection::v4Structured15dGridWithCrossSectionCrossSection(const QString& name, v4Structured15dGridWithCrossSection* grid) :
	m_name {name},
	m_altitudeInfo {},
	m_grid {grid}
{}

v4Structured15dGridWithCrossSection* v4Structured15dGridWithCrossSectionCrossSection::grid() const
{
	return m_grid;
}

const QString& v4Structured15dGridWithCrossSectionCrossSection::name()
{
	return m_name;
}

const std::vector<v4Structured15dGridWithCrossSectionCrossSection::Altitude>& v4Structured15dGridWithCrossSectionCrossSection::altitudeInfo() const
{
	return m_altitudeInfo;
}

std::vector<v4Structured15dGridWithCrossSectionCrossSection::Altitude>& v4Structured15dGridWithCrossSectionCrossSection::altitudeInfo()
{
	return m_altitudeInfo;
}

void v4Structured15dGridWithCrossSectionCrossSection::setAltitudeInfo(const std::vector<Altitude> alt)
{
	m_altitudeInfo = alt;
}
