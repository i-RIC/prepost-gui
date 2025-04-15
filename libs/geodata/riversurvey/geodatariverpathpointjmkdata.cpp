#include "geodatariverpathpointjmkdata.h"

GeoDataRiverPathPointJmkData::Item::Item() :
	distance {0},
	width {0},
	height {0},
	submerged {0},
	dense {0},
	dead {0},
	highLow {0},
	lowBranchHeight {0}
{}

bool GeoDataRiverPathPointJmkData::Item::operator<(const Item& item) const
{
	return distance < item.distance;
}

GeoDataRiverPathPointJmkData::GeoDataRiverPathPointJmkData() :
	m_items {}
{}

const std::vector<GeoDataRiverPathPointJmkData::Item>& GeoDataRiverPathPointJmkData::items() const
{
	return m_items;
}

std::vector<GeoDataRiverPathPointJmkData::Item>& GeoDataRiverPathPointJmkData::items()
{
	return m_items;
}
