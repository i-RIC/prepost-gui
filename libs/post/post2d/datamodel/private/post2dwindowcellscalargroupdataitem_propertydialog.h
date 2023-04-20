#ifndef POST2DWINDOWCELLSCALARGROUPDATAITEM_PROPERTYDIALOG_H
#define POST2DWINDOWCELLSCALARGROUPDATAITEM_PROPERTYDIALOG_H

#include "../post2dwindowcellscalargroupdataitem.h"

#include <QDialog>

namespace Ui {
class Post2dWindowCellScalarGroupDataItem_PropertyDialog;
}

class ModifyCommandWidget;

class Post2dWindowCellScalarGroupDataItem::PropertyDialog : public QDialog
{
	Q_OBJECT

public:
	PropertyDialog(Post2dWindowCellScalarGroupDataItem* item, QWidget *parent = nullptr);
	~PropertyDialog();

	QUndoCommand* createModifyCommand(bool apply);

	void accept() override;
	void reject() override;

private slots:
	void handleButtonClick(QAbstractButton* button);

private:
	void apply();

	bool m_applied;

	ModifyCommandWidget* m_colorMapWidget;
	Post2dWindowCellScalarGroupDataItem* m_item;

	Ui::Post2dWindowCellScalarGroupDataItem_PropertyDialog *ui;
};

#endif // POST2DWINDOWCELLSCALARGROUPDATAITEM_PROPERTYDIALOG_H
