#ifndef PREPROCESSORGRIDATTRIBUTEJEDGEDATAITEM_H
#define PREPROCESSORGRIDATTRIBUTEJEDGEDATAITEM_H

#include "preprocessorgridattributeabstractcelldataitem.h"

class PreProcessorGridAttributeJEdgeDataItem : public PreProcessorGridAttributeAbstractCellDataItem
{
	Q_OBJECT

public:
	PreProcessorGridAttributeJEdgeDataItem(SolverDefinitionGridAttribute* cond, GraphicsWindowDataItem* parent);

private:
	PreProcessorGridDataItem::SelectedDataWithIdController* selectedDataController() const override;
	QString positionCaption() const override;
};

#endif // PREPROCESSORGRIDATTRIBUTEJEDGEDATAITEM_H
