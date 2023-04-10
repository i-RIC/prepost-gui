#ifndef POST2DWINDOWGRIDSHAPEDATAITEM_PROPERTYDIALOG_H
#define POST2DWINDOWGRIDSHAPEDATAITEM_PROPERTYDIALOG_H

#include "../post2dwindowgridshapedataitem.h"

#include <QDialog>

namespace Ui {
class Post2dWindowGridShapeDataItem_PropertyDialog;
}

class Post2dWindowGridShapeDataItem::PropertyDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PropertyDialog(Post2dWindowGridShapeDataItem* item, QWidget *parent);
	~PropertyDialog();

	QUndoCommand* createModifyCommand(bool apply = false);

	void accept() override;
	void reject() override;

private slots:
	void handleButtonClick(QAbstractButton* button);

private:
	void apply();

	bool m_applied;
	Post2dWindowGridShapeDataItem* m_item;

	Ui::Post2dWindowGridShapeDataItem_PropertyDialog *ui;
};

#endif // POST2DWINDOWGRIDSHAPEDATAITEM_PROPERTYDIALOG_H
