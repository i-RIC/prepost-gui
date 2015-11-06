#ifndef PREABSTRACTGEODATA_H
#define PREABSTRACTGEODATA_H

#include <guicore/project/projectdataitem.h>

/// This class is the abstract class that defines the interface
/// of GeoData containers, those contain raw data like DEM data,
/// river survey data.
class PreAbstractGeoData : public ProjectDataItem
{
public:
	/// Constructor
	PreAbstractGeoData(ProjectDataItem* parent);
};

#endif // PREABSTRACTGEODATA_H
