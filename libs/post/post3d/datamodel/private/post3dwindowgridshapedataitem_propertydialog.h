#ifndef POST3DWINDOWGRIDSHAPEDATAITEM_PROPERTYDIALOG_H
#define POST3DWINDOWGRIDSHAPEDATAITEM_PROPERTYDIALOG_H

#include "../post3dwindowgridshapedataitem.h"

#include <QDialog>

namespace Ui {
class Post3dWindowGridShapeDataItem_PropertyDialog;
}

class Post3dWindowGridShapeDataItem::PropertyDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PropertyDialog(Post3dWindowGridShapeDataItem* item, QWidget *parent);
	~PropertyDialog();

	QUndoCommand* createModifyCommand(bool apply = false);

	void accept() override;
	void reject() override;

private slots:
	void handleButtonClick(QAbstractButton* button);

private:
	void apply();

	bool m_applied;
	Post3dWindowGridShapeDataItem* m_item;

private:
	Ui::Post3dWindowGridShapeDataItem_PropertyDialog *ui;
};

#endif // POST3DWINDOWGRIDSHAPEDATAITEM_PROPERTYDIALOG_H
