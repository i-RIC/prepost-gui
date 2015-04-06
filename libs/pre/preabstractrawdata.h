#ifndef PREABSTRACTRAWDATA_H
#define PREABSTRACTRAWDATA_H

#include <guicore/project/projectdataitem.h>

/// This class is the abstract class that defines the interface
/// of RawData containers, those contain raw data like DEM data,
/// river survey data.
class PreAbstractRawData : public ProjectDataItem
{
public:
	/// Constructor
	PreAbstractRawData(ProjectDataItem* parent);
};

#endif // PREABSTRACTRAWDATA_H
