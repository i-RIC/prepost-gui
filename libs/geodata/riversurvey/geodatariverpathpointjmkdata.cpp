#include "geodatariverpathpointjmkdata.h"

#include <yaml-cpp/yaml.h>

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
	for (auto it = node.begin(); it != node.end(); ++it) {
		auto i = *it;

		Item item;
		item.distance = i["distance"].as<double>();
		item.width = i["width"].as<double>();
		item.height = i["height"].as<double>();
		item.submerged = i["submerged"].as<int>();
		item.dense = i["dense"].as<int>();
		item.dead = i["dead"].as<int>();
		item.highLow = i["highLow"].as<int>();
		item.lowBranchHeight = i["lowBranchHeight"].as<double>();
		m_items.push_back(item);
	}
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
