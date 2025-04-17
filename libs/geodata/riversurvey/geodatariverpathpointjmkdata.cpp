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

void GeoDataRiverPathPointJmkData::importFromYaml(const YAML::Node& node)
{
}

void GeoDataRiverPathPointJmkData::exportToYaml(QTextStream* stream, const QString& head)
{
	for (const auto& item : m_items) {
		*stream << head << "-" << "\n";
		*stream << head << "  distance: " << item.distance << "\n";
		*stream << head << "  width: " << item.width << "\n";
		*stream << head << "  height: " << item.height << "\n";
		*stream << head << "  submerged: " << item.submerged << "\n";
		*stream << head << "  dense: " << item.dense << "\n";
		*stream << head << "  dead: " << item.dead << "\n";
		*stream << head << "  highLow: " << item.highLow << "\n";
		*stream << head << "  lowBranchHeight: " << item.lowBranchHeight << "\n";
	}
}
