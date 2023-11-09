#ifndef PREPROCESSORHYDRAULICDATADATAITEMI_H
#define PREPROCESSORHYDRAULICDATADATAITEMI_H

#include "../../guicore_global.h"
#include "preprocessordataitem.h"

class HydraulicData;

class GUICOREDLL_EXPORT PreProcessorHydraulicDataDataItemI : public PreProcessorDataItem
{
public:
	PreProcessorHydraulicDataDataItemI(PreProcessorDataItem* parent);
	virtual ~PreProcessorHydraulicDataDataItemI();

	virtual HydraulicData* hydraulicData() const = 0;
	virtual void setHydraulicData(HydraulicData* hydraulicData) = 0;

  friend class HydraulicData;
};

#endif // PREPROCESSORHYDRAULICDATADATAITEMI_H
