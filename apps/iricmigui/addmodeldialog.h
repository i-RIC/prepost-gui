#ifndef ADDMODELDIALOG_H
#define ADDMODELDIALOG_H

#include <QDialog>

namespace Ui {
class AddModelDialog;
}

class AddModelDialog : public QDialog
{
	Q_OBJECT

public:
	explicit AddModelDialog(QWidget *parent = nullptr);
	~AddModelDialog();

private:
	Ui::AddModelDialog *ui;
};

#endif // ADDMODELDIALOG_H
