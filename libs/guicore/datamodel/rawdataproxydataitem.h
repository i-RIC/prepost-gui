#ifndef RAWDATAPROXYDATAITEM_H
#define RAWDATAPROXYDATAITEM_H

#include "../guicore_global.h"

#include "graphicswindowdataitem.h"

class RawDataProxy;

class GUICOREDLL_EXPORT RawDataProxyDataItem : public GraphicsWindowDataItem
{
public:
	RawDataProxyDataItem(GraphicsWindowDataItem* parent);

	friend class RawDataProxy;
};

#endif // RAWDATAPROXYDATAITEM_H
