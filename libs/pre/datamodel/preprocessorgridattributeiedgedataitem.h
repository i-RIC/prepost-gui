#ifndef PREPROCESSORGRIDATTRIBUTEIEDGEDATAITEM_H
#define PREPROCESSORGRIDATTRIBUTEIEDGEDATAITEM_H

#include "preprocessorgridattributeabstractcelldataitem.h"

class PreProcessorGridAttributeIEdgeDataItem : public PreProcessorGridAttributeAbstractCellDataItem
{
	Q_OBJECT

public:
	PreProcessorGridAttributeIEdgeDataItem(SolverDefinitionGridAttribute* cond, GraphicsWindowDataItem* parent);

private:
	PreProcessorGridDataItem::SelectedDataWithIdController* selectedDataController() const override;
	QString positionCaption() const override;
};

#endif // PREPROCESSORGRIDATTRIBUTEIEDGEDATAITEM_H
