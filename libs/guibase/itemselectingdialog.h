#ifndef ITEMSELECTINGDIALOG_H
#define ITEMSELECTINGDIALOG_H

#include "guibase_global.h"

#include <QDialog>
#include <QList>

class QComboBox;
class QString;

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
	template <typename STRCONTAINER>
	void setItems(const STRCONTAINER& items);
	/// Set the selected item by index in the items
	void setSelectedIndex(int index);
	/// The index of the selected item
	int selectedIndex();

private:
	QComboBox* comboBox() const;

	Ui::ItemSelectingDialog* ui;
};

#include "private/itemselectingdialog_detail.h"

#endif // ITEMSELECTINGDIALOG_H
