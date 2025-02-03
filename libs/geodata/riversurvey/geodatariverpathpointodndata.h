#ifndef GEODATARIVERPATHPOINTODNDATA_H
#define GEODATARIVERPATHPOINTODNDATA_H

#include "gd_riversurvey_global.h"

class GD_RIVERSURVEY_EXPORT GeoDataRiverPathPointOdnData
{
public:
	GeoDataRiverPathPointOdnData();
	int nb(int index) const;
	void setNb(int index, int value);

	double spanDistance() const;
	void setSpanDistance(double distance);

private:
	int m_nb[6];
	double m_spanDistance;
};

#endif // GEODATARIVERPATHPOINTODNDATA_H
