#ifndef PREPROCESSORHYDRAULICDATAGROUPDATAITEMINTERFACE_H
#define PREPROCESSORHYDRAULICDATAGROUPDATAITEMINTERFACE_H

#include "../../guicore_global.h"
#include "preprocessordataitem.h"

class HydraulicDataCreator;
class PreProcessorHydraulicDataDataItemInterface;

class GUICOREDLL_EXPORT PreProcessorHydraulicDataGroupDataItemInterface : public PreProcessorDataItem
{
public:
	PreProcessorHydraulicDataGroupDataItemInterface(PreProcessorDataItem* parent);
	virtual ~PreProcessorHydraulicDataGroupDataItemInterface();

	virtual HydraulicDataCreator* creator() const = 0;
	virtual const QList<PreProcessorHydraulicDataDataItemInterface*> hydraulicDatas() const = 0;
	virtual void moveUpItem(int index) = 0;
	virtual void moveDownItem(int index) = 0;
	virtual void deleteItem(int index) = 0;
	virtual void addImportAction(QMenu* menu) = 0;
};

#endif // PREPROCESSORHYDRAULICDATAGROUPDATAITEMINTERFACE_H
