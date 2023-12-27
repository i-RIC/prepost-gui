#ifndef PREPROCESSORGRIDDATAITEMI_H
#define PREPROCESSORGRIDDATAITEMI_H

#include "../../guicore_global.h"
#include "preprocessordataitem.h"

#include <QIcon>

class v4InputGrid;
class GridAttributeContainer;
class PreProcessorBCGroupDataItem;

class GUICOREDLL_EXPORT PreProcessorGridDataItemI : public PreProcessorDataItem
{

public:
	PreProcessorGridDataItemI(PreProcessorDataItem* parent);

	QString subPath() const;

	virtual void updateSimplifiedGrid(VTKGraphicsView* v = nullptr) = 0;
	virtual void informGridAttributeChange(const std::string& name) = 0;
	virtual void updateAttributeActorSettings() = 0;
	virtual v4InputGrid* grid() const = 0;
	virtual bool setGrid(v4InputGrid* newGrid) = 0;
	virtual void silentDeleteGrid() = 0;
	virtual void setBCGroupDataItem(PreProcessorBCGroupDataItem* item) = 0;
	virtual void unsetBCGroupDataItem() = 0;
	virtual bool gridIsDeleted() const = 0;
	virtual void updateActionStatus() = 0;
	virtual void applyColorMapSetting(const std::string& name) = 0;

	friend class GridAttributeContainer;
};

#endif // PREPROCESSORGRIDDATAITEM_H
