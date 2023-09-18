#ifndef PREPROCESSORGRIDATTRIBUTECELLDATAITEM_H
#define PREPROCESSORGRIDATTRIBUTECELLDATAITEM_H

#include "preprocessorgridattributeabstractcelldataitem.h"

class PreProcessorGridAttributeCellDataItem : public PreProcessorGridAttributeAbstractCellDataItem
{
	Q_OBJECT

public:
	PreProcessorGridAttributeCellDataItem(SolverDefinitionGridAttribute* cond, GraphicsWindowDataItem* parent);
	QDialog* propertyDialog(QWidget* parent) override;

private:
	PreProcessorGridDataItem::SelectedDataWithIdController* selectedDataController() const override;
	QString positionCaption() const override;
};

#endif // PREPROCESSORGRIDATTRIBUTECELLDATAITEM_H
