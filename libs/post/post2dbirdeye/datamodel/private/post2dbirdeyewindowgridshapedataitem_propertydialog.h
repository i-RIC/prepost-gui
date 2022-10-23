#ifndef POST2DWINDOWGRIDSHAPEDATAITEM_PROPERTYDIALOG_H
#define POST2DWINDOWGRIDSHAPEDATAITEM_PROPERTYDIALOG_H

#include "../post2dbirdeyewindowgridshapedataitem.h"

#include <QDialog>

#include <string>
#include <vector>

namespace Ui {
class Post2dBirdEyeWindowGridShapeDataItem_PropertyDialog;
}

class Post2dBirdEyeWindowGridShapeDataItem::PropertyDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PropertyDialog(Post2dBirdEyeWindowGridShapeDataItem* item, QWidget *parent);
	~PropertyDialog();

	QUndoCommand* createModifyCommand(bool apply = false);

	void accept() override;
	void reject() override;

private slots:
	void handleButtonClick(QAbstractButton* button);

private:
	void apply();

	bool m_applied;
	Post2dBirdEyeWindowGridShapeDataItem* m_item;
	std::vector<std::string> m_valueNames;

private:
	Ui::Post2dBirdEyeWindowGridShapeDataItem_PropertyDialog *ui;
};

#endif // POST2DWINDOWGRIDSHAPEDATAITEM_PROPERTYDIALOG_H
