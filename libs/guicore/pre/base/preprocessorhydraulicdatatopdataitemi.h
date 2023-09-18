#ifndef PREPROCESSORHYDRAULICDATATOPDATAITEMI_H
#define PREPROCESSORHYDRAULICDATATOPDATAITEMI_H

#include "../../guicore_global.h"
#include "preprocessordataitem.h"

class PreProcessorHydraulicDataGroupDataItemI;
class SolverDefinitionGridType;

class GUICOREDLL_EXPORT PreProcessorHydraulicDataTopDataItemI : public PreProcessorDataItem
{
public:
	PreProcessorHydraulicDataTopDataItemI(GraphicsWindowDataItem* parent);
	virtual ~PreProcessorHydraulicDataTopDataItemI();

	virtual QList<PreProcessorHydraulicDataGroupDataItemI*> groupDataItems() const = 0;
	virtual PreProcessorHydraulicDataGroupDataItemI* groupDataItem(const std::string& name) const = 0;
};

#endif // PREPROCESSORHYDRAULICDATATOPDATAITEMI_H
