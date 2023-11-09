#ifndef PREPROCESSORGEODATATOPDATAITEMI_H
#define PREPROCESSORGEODATATOPDATAITEMI_H

#include "../../guicore_global.h"
#include "preprocessordataitem.h"
#include <QList>

class SolverDefinitionGridType;
class PreProcessorGeoDataGroupDataItemI;

class GUICOREDLL_EXPORT PreProcessorGeoDataTopDataItemI : public PreProcessorDataItem
{

public:
	PreProcessorGeoDataTopDataItemI(const QString& itemlabel, const QIcon& icon, GraphicsWindowDataItem* parent);
	virtual ~PreProcessorGeoDataTopDataItemI();

	virtual SolverDefinitionGridType* gridType() const = 0;
	virtual const QList<PreProcessorGeoDataGroupDataItemI*> groupDataItems() const = 0;
	virtual PreProcessorGeoDataGroupDataItemI* groupDataItem(const std::string& name) const = 0;
	/// Set all dimensions to the first.
	virtual void setDimensionsToFirst() = 0;
	virtual void clearDimensionsIfNoDataExists() = 0;
};

#endif // PREPROCESSORGEODATATOPDATAITEMI_H
