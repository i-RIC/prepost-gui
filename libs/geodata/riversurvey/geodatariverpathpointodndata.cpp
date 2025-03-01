#include "geodatariverpathpointodndata.h"

GeoDataRiverPathPointOdnData::GeoDataRiverPathPointOdnData() :
	m_nb {-1, -1, -1, -1, -1, -1},
	m_spanDistance {0}
{}

int GeoDataRiverPathPointOdnData::nb(int index) const
{
	if (index < 0 || index > 5) {return 0;}

	return m_nb[index];
}

void GeoDataRiverPathPointOdnData::setNb(int index, int value)
{
	if (index < 0 || index > 5) {return;}

	m_nb[index] = value;
}

double GeoDataRiverPathPointOdnData::spanDistance() const
{
	return m_spanDistance;
}

void GeoDataRiverPathPointOdnData::setSpanDistance(double distance)
{
	m_spanDistance = distance;
}
