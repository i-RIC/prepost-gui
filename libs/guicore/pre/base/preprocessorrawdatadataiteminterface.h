#ifndef PREPROCESSORRAWDATADATAITEMINTERFACE_H
#define PREPROCESSORRAWDATADATAITEMINTERFACE_H

#include "preprocessordataitem.h"
class RawData;
class QStandardItem;

class PreProcessorRawdataDataItemInterface : public PreProcessorDataItem
{

public:
	PreProcessorRawdataDataItemInterface(const QString& itemlabel, const QIcon& icon, GraphicsWindowDataItem* parent)
		: PreProcessorDataItem(itemlabel, icon, parent)
	{}
	virtual RawData* rawData() = 0;
	virtual void informValueRangeChange() = 0;
	virtual void informDataChange() = 0;

	friend class RawData;
};

#endif // PREPROCESSORRAWDATADATAITEMINTERFACE_H
