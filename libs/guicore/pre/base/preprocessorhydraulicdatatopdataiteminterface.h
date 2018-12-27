#ifndef PREPROCESSORHYDRAULICDATATOPDATAITEMINTERFACE_H
#define PREPROCESSORHYDRAULICDATATOPDATAITEMINTERFACE_H

#include "../../guicore_global.h"
#include "preprocessordataitem.h"

class PreProcessorHydraulicDataGroupDataItemInterface;
class SolverDefinitionGridType;

class GUICOREDLL_EXPORT PreProcessorHydraulicDataTopDataItemInterface : public PreProcessorDataItem
{
public:
	PreProcessorHydraulicDataTopDataItemInterface(GraphicsWindowDataItem* parent);
	virtual ~PreProcessorHydraulicDataTopDataItemInterface();

	virtual QList<PreProcessorHydraulicDataGroupDataItemInterface*> groupDataItems() const = 0;
	virtual PreProcessorHydraulicDataGroupDataItemInterface* groupDataItem(const std::string& name) const = 0;
};

#endif // PREPROCESSORHYDRAULICDATATOPDATAITEMINTERFACE_H
