#ifndef GEODATAPROXYDATAITEM_H
#define GEODATAPROXYDATAITEM_H

#include "../guicore_global.h"

#include "graphicswindowdataitem.h"

class GeoDataProxy;

class GUICOREDLL_EXPORT GeoDataProxyDataItem : public GraphicsWindowDataItem
{

public:
	GeoDataProxyDataItem(GraphicsWindowDataItem* parent);

	friend class GeoDataProxy;
};

#endif // GEODATAPROXYDATAITEM_H
