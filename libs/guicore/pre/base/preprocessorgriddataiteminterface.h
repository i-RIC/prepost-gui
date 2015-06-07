#ifndef PREPROCESSORGRIDDATAITEMINTERFACE_H
#define PREPROCESSORGRIDDATAITEMINTERFACE_H

#include "../../guicore_global.h"
#include "preprocessordataitem.h"

class Grid;
class GridAttributeContainer;

class GUICOREDLL_EXPORT PreProcessorGridDataItemInterface : public PreProcessorDataItem
{

public:
	PreProcessorGridDataItemInterface(PreProcessorDataItem* parent)
		: PreProcessorDataItem(PreProcessorDataItem::tr("Grid"), QIcon(":/libs/guibase/images/iconFolder.png"), parent)
	{}
	virtual void updateSimplifiedGrid(VTKGraphicsView* v = nullptr) = 0;
	virtual void informgridRelatedConditionChange(const QString& name) = 0;
	virtual void updateAttributeActorSettings() = 0;
	virtual Grid* grid() const = 0;
	virtual bool setGrid(Grid* newGrid) = 0;
	virtual void silentDeleteGrid() = 0;

	friend class Grid;
	friend class GridAttributeContainer;
};

#endif // PREPROCESSORGRIDDATAITEM_H
