#ifndef MAPPINGSETTINGDIALOG_H
#define MAPPINGSETTINGDIALOG_H

#include <QDialog>

namespace Ui {
class MappingSettingDialog;
}

class MappingSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit MappingSettingDialog(QWidget *parent = 0);
	~MappingSettingDialog();

private:
	Ui::MappingSettingDialog *ui;
};

#endif // MAPPINGSETTINGDIALOG_H
