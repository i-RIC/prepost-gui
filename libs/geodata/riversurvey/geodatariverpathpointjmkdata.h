#ifndef GEODATARIVERPATHPOINTJMKDATA_H
#define GEODATARIVERPATHPOINTJMKDATA_H

#include "gd_riversurvey_global.h"

class GD_RIVERSURVEY_EXPORT GeoDataRiverPathPointJmkData
{
public:
	struct Item {
		Item();

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

private:
	std::vector<Item> m_items;
};

#endif // GEODATARIVERPATHPOINTJMKDATA_H
