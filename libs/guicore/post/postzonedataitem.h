#ifndef POSTZONEDATAITEM_H
#define POSTZONEDATAITEM_H

#include "../guicore_global.h"

class PostZoneDataContainer;

class GUICOREDLL_EXPORT PostZoneDataItem
{
public:
	virtual ~PostZoneDataItem();

	virtual PostZoneDataContainer* dataContainer() = 0;
};

#endif // POSTZONEDATAITEM_H
