#ifndef PREPROCESSORRAWDATATOPDATAITEMINTERFACE_H
#define PREPROCESSORRAWDATATOPDATAITEMINTERFACE_H

#include "../../guicore_global.h"
#include "preprocessordataitem.h"
#include <QList>

class SolverDefinitionGridType;
class PreProcessorRawDataGroupDataItemInterface;

class GUICOREDLL_EXPORT PreProcessorRawDataTopDataItemInterface : public PreProcessorDataItem
{

public:
	PreProcessorRawDataTopDataItemInterface(const QString& itemlabel, const QIcon& icon, GraphicsWindowDataItem* parent);
	virtual ~PreProcessorRawDataTopDataItemInterface();
	virtual SolverDefinitionGridType* gridType() = 0;
	virtual const QList<PreProcessorRawDataGroupDataItemInterface*> groupDataItems() const = 0;
	virtual PreProcessorRawDataGroupDataItemInterface* groupDataItem(const QString& name) = 0;
	/// Set all dimensions to the first.
	virtual void setDimensionsToFirst() = 0;
};

#endif // PREPROCESSORRAWDATATOPDATAITEMINTERFACE_H
