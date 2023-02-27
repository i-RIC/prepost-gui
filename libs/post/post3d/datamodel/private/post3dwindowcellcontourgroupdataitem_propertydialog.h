#ifndef POST3DWINDOWCELLCONTOURGROUPDATAITEM_PROPERTYDIALOG_H
#define POST3DWINDOWCELLCONTOURGROUPDATAITEM_PROPERTYDIALOG_H

#include "../post3dwindowcellcontourgroupdataitem.h"

#include <QDialog>

namespace Ui {
class Post3dWindowCellContourGroupDataItem_PropertyDialog;
}

class QAbstractButton;
class QUndoCommand;

class Post3dWindowCellContourGroupDataItem::PropertyDialog : public QDialog
{
	Q_OBJECT

public:
	PropertyDialog(Post3dWindowCellContourGroupDataItem* item, QWidget *parent);
	~PropertyDialog();

	QUndoCommand* createModifyCommand(bool apply);

	void accept() override;
	void reject() override;

private slots:
	void handleButtonClick(QAbstractButton* button);

private:
	void apply();

	bool m_applied;

	Post3dWindowCellContourGroupDataItem* m_item;
	Ui::Post3dWindowCellContourGroupDataItem_PropertyDialog *ui;
};

#endif // POST3DWINDOWCELLCONTOURGROUPDATAITEM_PROPERTYDIALOG_H
