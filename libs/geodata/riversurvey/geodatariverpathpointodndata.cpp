#include "geodatariverpathpointodndata.h"

#include <yaml-cpp/yaml.h>

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

void GeoDataRiverPathPointOdnData::importFromYaml(const YAML::Node& node)
{
	auto nb = node["nb"];
	for (int i = 0; i < 6; ++i) {
		m_nb[i] = nb[i].as<int>();
	}
	node["nb"].IsSequence();
	m_spanDistance = node["spanDistance"].as<double>();
}

void GeoDataRiverPathPointOdnData::exportToYaml(QTextStream* stream, const QString& head)
{
	QStringList nb_strs;
	for (const auto& nb : m_nb) {
		nb_strs.push_back(QString::number(nb));
	}

	*stream << head << "nb: [" << nb_strs.join(", ") << "]" << "\n";
	*stream << head << "spanDistance: " << m_spanDistance << "\n";
}

