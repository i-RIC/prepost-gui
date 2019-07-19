#include "ui_vtktextpropertysettingdialog.h"

#include "vtktextpropertysettingcontainer.h"
#include "vtktextpropertysettingdialog.h"

vtkTextPropertySettingDialog::vtkTextPropertySettingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::vtkTextPropertySettingDialog)
{
	ui->setupUi(this);
}

vtkTextPropertySettingDialog::~vtkTextPropertySettingDialog()
{
	delete ui;
}

vtkTextPropertySettingContainer vtkTextPropertySettingDialog::setting() const
{
	return ui->widget->setting();
}

void vtkTextPropertySettingDialog::setSetting(const vtkTextPropertySettingContainer& setting)
{
	ui->widget->setSetting(setting);
}

void vtkTextPropertySettingDialog::disableSize()
{
	ui->widget->disableSize();
}

void vtkTextPropertySettingDialog::disableColor()
{
	ui->widget->disableColor();
}
