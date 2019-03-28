#include "ui_postpolydatabasicsettingdialog.h"

#include "postpolydatabasicsetting.h"
#include "postpolydatabasicsettingdialog.h"

PostPolyDataBasicSettingDialog::PostPolyDataBasicSettingDialog(QWidget* parent) :
	QDialog {parent},
	ui {new Ui::PostPolyDataBasicSettingDialog}
{
	ui->setupUi(this);
}

PostPolyDataBasicSettingDialog::~PostPolyDataBasicSettingDialog()
{
	delete ui;
}

PostPolyDataBasicSetting PostPolyDataBasicSettingDialog::setting() const
{
	PostPolyDataBasicSetting ret;

	ret.color = ui->colorWidget->color();
	ret.lineWidth = ui->widthSpinBox->value();

	return ret;
}

void PostPolyDataBasicSettingDialog::setSetting(const PostPolyDataBasicSetting& s)
{
	ui->colorWidget->setColor(s.color);
	ui->widthSpinBox->setValue(s.lineWidth);
}
