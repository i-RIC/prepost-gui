#ifndef POSTZONEDATAITEM_H
#define POSTZONEDATAITEM_H

#include "../guicore_global.h"

class PostZoneDataContainer;
class v4PostZoneDataContainer;

class GUICOREDLL_EXPORT PostZoneDataItem
{
public:
	virtual ~PostZoneDataItem();

	virtual v4PostZoneDataContainer* v4DataContainer() = 0;
};

#endif // POSTZONEDATAITEM_H
