#ifndef PREPROCESSORGEODATATOPDATAITEMINTERFACE_H
#define PREPROCESSORGEODATATOPDATAITEMINTERFACE_H

#include "../../guicore_global.h"
#include "preprocessordataitem.h"
#include <QList>

class SolverDefinitionGridType;
class PreProcessorGeoDataGroupDataItemInterface;

class GUICOREDLL_EXPORT PreProcessorGeoDataTopDataItemInterface : public PreProcessorDataItem
{

public:
	PreProcessorGeoDataTopDataItemInterface(const QString& itemlabel, const QIcon& icon, GraphicsWindowDataItem* parent);
	virtual ~PreProcessorGeoDataTopDataItemInterface();

	virtual SolverDefinitionGridType* gridType() const = 0;
	virtual const QList<PreProcessorGeoDataGroupDataItemInterface*> groupDataItems() const = 0;
	virtual PreProcessorGeoDataGroupDataItemInterface* groupDataItem(const std::string& name) const = 0;
	/// Set all dimensions to the first.
	virtual void setDimensionsToFirst() = 0;
};

#endif // PREPROCESSORGEODATATOPDATAITEMINTERFACE_H
