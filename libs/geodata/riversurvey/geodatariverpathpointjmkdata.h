#ifndef GEODATARIVERPATHPOINTJMKDATA_H
#define GEODATARIVERPATHPOINTJMKDATA_H

#include "gd_riversurvey_global.h"

namespace YAML {
	class Node;
} // namespace YAML

class GD_RIVERSURVEY_EXPORT GeoDataRiverPathPointJmkData
{
public:
	struct Item {
		Item();
		bool operator<(const Item& item) const;

		double distance;
		double width;
		double height;
		int submerged;
		int dense;
		int dead;
		int highLow;
		double lowBranchHeight;
	};

	GeoDataRiverPathPointJmkData();

	const std::vector<Item>& items() const;
	std::vector<Item>& items();

	void importFromYaml(const YAML::Node& node);
	void exportToYaml(QTextStream* stream, const QString& head);

private:
	std::vector<Item> m_items;
};

#endif // GEODATARIVERPATHPOINTJMKDATA_H
