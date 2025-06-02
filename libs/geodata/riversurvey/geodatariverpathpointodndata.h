#ifndef GEODATARIVERPATHPOINTODNDATA_H
#define GEODATARIVERPATHPOINTODNDATA_H

#include "gd_riversurvey_global.h"

namespace YAML {
	class Node;
} // namespace YAML

class GD_RIVERSURVEY_EXPORT GeoDataRiverPathPointOdnData
{
public:
	GeoDataRiverPathPointOdnData();
	int nb(int index) const;
	void setNb(int index, int value);

	double spanDistance() const;
	void setSpanDistance(double distance);

	void importFromYaml(const YAML::Node& node);
	void exportToYaml(QTextStream* stream, const QString& head);

private:
	int m_nb[6];
	double m_spanDistance;
};

#endif // GEODATARIVERPATHPOINTODNDATA_H
