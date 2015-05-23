#ifndef ITEMSELECTINGDIALOG_H
#define ITEMSELECTINGDIALOG_H

#include "guibase_global.h"

#include <QDialog>
#include <QList>

namespace Ui
{
	class ItemSelectingDialog;
}

/// Dialog to select item using ComboBox
class GUIBASEDLL_EXPORT ItemSelectingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ItemSelectingDialog(QWidget* parent = nullptr);
	~ItemSelectingDialog();
	/// Set the message shown on the dialog
	void setMessage(const QString& message);
	/// Set the items to select
	void setItems(const QList<QString>& items);
	/// Set the selected item by index in the items
	void setSelectIndex(int index);
	/// The index of the selected item
	int selectIndex();

private:
	Ui::ItemSelectingDialog* ui;
};

#endif // ITEMSELECTINGDIALOG_H
