#ifndef ITEMSELECTINGDIALOG_H
#define ITEMSELECTINGDIALOG_H

#include "guibase_global.h"

#include <QDialog>
#include <QList>

namespace Ui {
	class ItemSelectingDialog;
}

class GUIBASEDLL_EXPORT ItemSelectingDialog : public QDialog
{
	Q_OBJECT
public:
	explicit ItemSelectingDialog(QWidget *parent = 0);
	~ItemSelectingDialog();
	void setMessage(const QString& message);
	void setItems(const QList<QString>& items);
	void setSelectIndex(int index);
	int selectIndex();

private:
	Ui::ItemSelectingDialog *ui;
};

#endif // ITEMSELECTINGDIALOG_H
