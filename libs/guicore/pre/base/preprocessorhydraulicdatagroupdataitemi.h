#ifndef PREPROCESSORHYDRAULICDATAGROUPDATAITEMI_H
#define PREPROCESSORHYDRAULICDATAGROUPDATAITEMI_H

#include "../../guicore_global.h"
#include "preprocessordataitem.h"

class HydraulicDataCreator;
class PreProcessorHydraulicDataDataItemI;

class GUICOREDLL_EXPORT PreProcessorHydraulicDataGroupDataItemI : public PreProcessorDataItem
{
public:
	PreProcessorHydraulicDataGroupDataItemI(PreProcessorDataItem* parent);
	virtual ~PreProcessorHydraulicDataGroupDataItemI();

	virtual HydraulicDataCreator* creator() const = 0;
	virtual const QList<PreProcessorHydraulicDataDataItemI*> hydraulicDatas() const = 0;
	virtual void moveUpItem(int index) = 0;
	virtual void moveDownItem(int index) = 0;
	virtual void deleteItem(int index) = 0;
	virtual void addImportAction(QMenu* menu) = 0;
};

#endif // PREPROCESSORHYDRAULICDATAGROUPDATAITEMI_H
