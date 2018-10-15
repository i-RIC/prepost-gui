#ifndef PREPROCESSORGRIDDATAITEMINTERFACE_H
#define PREPROCESSORGRIDDATAITEMINTERFACE_H

#include "../../guicore_global.h"
#include "preprocessordataitem.h"

#include <QIcon>

class Grid;
class GridAttributeContainer;
class PreProcessorBCGroupDataItem;

class GUICOREDLL_EXPORT PreProcessorGridDataItemInterface : public PreProcessorDataItem
{

public:
	PreProcessorGridDataItemInterface(PreProcessorDataItem* parent) :
		PreProcessorDataItem(PreProcessorDataItem::tr("Grid"), QIcon(":/libs/guibase/images/iconFolder.png"), parent)
	{}
	virtual void updateSimplifiedGrid(VTKGraphicsView* v = nullptr) = 0;
	virtual void informGridAttributeChange(const std::string& name) = 0;
	virtual void updateAttributeActorSettings() = 0;
	virtual Grid* grid() const = 0;
	virtual bool setGrid(Grid* newGrid) = 0;
	virtual void silentDeleteGrid() = 0;
	virtual void setBCGroupDataItem(PreProcessorBCGroupDataItem* item) = 0;
	virtual void unsetBCGroupDataItem() = 0;
	virtual bool gridIsDeleted() const = 0;

	friend class Grid;
	friend class GridAttributeContainer;
};

#endif // PREPROCESSORGRIDDATAITEM_H
