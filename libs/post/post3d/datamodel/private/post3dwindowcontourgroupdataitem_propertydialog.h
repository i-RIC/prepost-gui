#ifndef POST3DWINDOWCONTOURGROUPDATAITEM_PROPERTYDIALOG_H
#define POST3DWINDOWCONTOURGROUPDATAITEM_PROPERTYDIALOG_H

#include "../post3dwindowcontourgroupdataitem.h"

#include <QDialog>

namespace Ui {
class Post3dWindowContourGroupDataItem_PropertyDialog;
}

class QAbstractButton;
class QUndoCommand;

class Post3dWindowContourGroupDataItem::PropertyDialog : public QDialog
{
	Q_OBJECT

public:
	PropertyDialog(Post3dWindowContourGroupDataItem* item, QWidget *parent);
	~PropertyDialog();

	QUndoCommand* createModifyCommand(bool apply);

	void accept() override;
	void reject() override;

private slots:
	void handleButtonClick(QAbstractButton* button);

private:
	void apply();

	bool m_applied;

	Post3dWindowContourGroupDataItem* m_item;
	Ui::Post3dWindowContourGroupDataItem_PropertyDialog *ui;
};

#endif // POST3DWINDOWCONTOURGROUPDATAITEM_PROPERTYDIALOG_H
