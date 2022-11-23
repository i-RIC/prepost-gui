#ifndef PREPROCESSORBCGROUPSETTINGDIALOG_H
#define PREPROCESSORBCGROUPSETTINGDIALOG_H

#include <QDialog>

namespace Ui {
class PreProcessorBcGroupSettingDialog;
}

class vtkTextPropertySettingContainer;

class PreProcessorBcGroupSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PreProcessorBcGroupSettingDialog(QWidget *parent = nullptr);
	~PreProcessorBcGroupSettingDialog();

	vtkTextPropertySettingContainer nameSetting() const;
	void setNameSetting(const vtkTextPropertySettingContainer& setting);

private:
	Ui::PreProcessorBcGroupSettingDialog *ui;
};

#endif // PREPROCESSORBCGROUPSETTINGDIALOG_H
