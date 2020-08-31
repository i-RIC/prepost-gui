#ifndef ITEMMULTISELECTINGDIALOG_H
#define ITEMMULTISELECTINGDIALOG_H

#include "../guibase_global.h"

#include <QDialog>
#include <QString>

#include <vector>

namespace Ui {
class ItemMultiSelectingDialog;
}

class QCheckBox;

class GUIBASEDLL_EXPORT ItemMultiSelectingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ItemMultiSelectingDialog(QWidget *parent = 0);
	~ItemMultiSelectingDialog();

	void setItems(const std::vector<QString>& items);
	std::vector<bool> selectSettings() const;

private slots:
	void checkAll();

private:
	std::vector<QCheckBox*> m_checkBoxes;
	Ui::ItemMultiSelectingDialog *ui;
};

#endif // ITEMMULTISELECTINGDIALOG_H
