#ifndef MAPPINGSETTINGDIALOG_H
#define MAPPINGSETTINGDIALOG_H

#include <QDialog>

class Project;

namespace Ui {
class MappingSettingDialog;
}

class MappingSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit MappingSettingDialog(QWidget *parent = 0);
	~MappingSettingDialog();

	void setProject(Project* project);

public slots:
	void accept() override;

private:
	Project* m_project;
	Ui::MappingSettingDialog *ui;
};

#endif // MAPPINGSETTINGDIALOG_H
