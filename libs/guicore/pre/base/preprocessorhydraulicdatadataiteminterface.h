#ifndef PREPROCESSORHYDRAULICDATADATAITEMINTERFACE_H
#define PREPROCESSORHYDRAULICDATADATAITEMINTERFACE_H

#include "../../guicore_global.h"
#include "preprocessordataitem.h"

class HydraulicData;

class GUICOREDLL_EXPORT PreProcessorHydraulicDataDataItemInterface : public PreProcessorDataItem
{
public:
	PreProcessorHydraulicDataDataItemInterface(PreProcessorDataItem* parent);
	virtual ~PreProcessorHydraulicDataDataItemInterface();

	virtual HydraulicData* hydraulicData() const = 0;
	virtual void setHydraulicData(HydraulicData* hydraulicData) = 0;

  friend class HydraulicData;
};

#endif // PREPROCESSORHYDRAULICDATADATAITEMINTERFACE_H
