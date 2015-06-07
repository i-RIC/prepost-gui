#include "ui_gridbirdeyewindowaxessettingdialog.h"

#include "gridbirdeyewindowaxessettingdialog.h"

GridBirdEyeWindowAxesSettingDialog::GridBirdEyeWindowAxesSettingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GridBirdEyeWindowAxesSettingDialog)
{
	ui->setupUi(this);
}

GridBirdEyeWindowAxesSettingDialog::~GridBirdEyeWindowAxesSettingDialog()
{
	delete ui;
}

void GridBirdEyeWindowAxesSettingDialog::setAxesVisible(bool visible)
{
	ui->visibleCheckBox->setChecked(visible);
}

bool GridBirdEyeWindowAxesSettingDialog::axesVisible()
{
	return ui->visibleCheckBox->isChecked();
}

void GridBirdEyeWindowAxesSettingDialog::setColor(const QColor& col)
{
	ui->colorWidget->setColor(col);
}

QColor GridBirdEyeWindowAxesSettingDialog::color()
{
	return ui->colorWidget->color();
}
