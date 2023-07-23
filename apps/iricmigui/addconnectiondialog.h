#ifndef ADDCONNECTIONDIALOG_H
#define ADDCONNECTIONDIALOG_H

#include <QDialog>

namespace Ui {
class AddConnectionDialog;
}

class AddConnectionDialog : public QDialog
{
	Q_OBJECT

public:
	explicit AddConnectionDialog(QWidget *parent = nullptr);
	~AddConnectionDialog();

private:
	Ui::AddConnectionDialog *ui;
};

#endif // ADDCONNECTIONDIALOG_H
