#ifndef POST2DWINDOWNODESCALARGROUPDATAITEM_PROPERTYDIALOG_H
#define POST2DWINDOWNODESCALARGROUPDATAITEM_PROPERTYDIALOG_H

#include "../post2dwindownodescalargroupdataitem.h"

#include <QDialog>

namespace Ui {
class Post2dWindowNodeScalarGroupDataItem_PropertyDialog;
}

class Post2dWindowNodeScalarGroupDataItem::PropertyDialog : public QDialog
{
	Q_OBJECT

public:
	PropertyDialog(Post2dWindowNodeScalarGroupDataItem* item, QWidget *parent = nullptr);
	~PropertyDialog();

	QUndoCommand* createModifyCommand(bool apply);

	void accept() override;
	void reject() override;

private slots:
	void handleButtonClick(QAbstractButton* button);

private:
	void apply();

	bool m_applied;

	Post2dWindowNodeScalarGroupDataItem* m_item;

	Ui::Post2dWindowNodeScalarGroupDataItem_PropertyDialog *ui;
};

#endif // POST2DWINDOWNODESCALARGROUPDATAITEM_PROPERTYDIALOG_H
