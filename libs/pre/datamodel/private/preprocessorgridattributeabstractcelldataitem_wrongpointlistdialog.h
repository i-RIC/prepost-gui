#ifndef PREPROCESSORGRIDATTRIBUTEABSTRACTCELLDATAITEM_WRONGPOINTLISTDIALOG_H
#define PREPROCESSORGRIDATTRIBUTEABSTRACTCELLDATAITEM_WRONGPOINTLISTDIALOG_H

#include "../preprocessorgridattributeabstractcelldataitem.h"

#include <QDialog>

#include <vector>

namespace Ui {
class PreProcessorGridAttributeAbstractCellDataItem_WrongPointListDialog;
}

class PreProcessorGridAttributeAbstractCellDataItem::WrongPointListDialog : public QDialog
{
	Q_OBJECT

public:
	WrongPointListDialog(PreProcessorGridAttributeAbstractCellDataItem* item, vtkIdType ds, QWidget *parent);
	~WrongPointListDialog();

	void setPointList(const std::vector<vtkIdType>& list);

public slots:
	void accept() override;

private slots:
	void recheck();
	void handleSelectChange(int row);

private:
	vtkIdType m_downstream;
	std::vector<vtkIdType> m_list;
	PreProcessorGridAttributeAbstractCellDataItem* m_item;
	Ui::PreProcessorGridAttributeAbstractCellDataItem_WrongPointListDialog *ui;
};

#endif // PREPROCESSORGRIDATTRIBUTEABSTRACTCELLDATAITEM_WRONGPOINTLISTDIALOG_H
