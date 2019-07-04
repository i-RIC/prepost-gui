#ifndef PROJECTTIMESETTINGDIALOG_H
#define PROJECTTIMESETTINGDIALOG_H

#include <QDialog>

namespace Ui {
class ProjectTimeSettingDialog;
}

class ProjectTimeSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ProjectTimeSettingDialog(QWidget *parent = nullptr);
	~ProjectTimeSettingDialog();

private:
	Ui::ProjectTimeSettingDialog *ui;
};

#endif // PROJECTTIMESETTINGDIALOG_H
