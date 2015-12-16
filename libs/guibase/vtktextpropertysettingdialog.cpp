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
	vtkTextPropertySettingContainer c;
	c.setPrefix(m_prefix);
	c.fontFamily = static_cast<vtkTextPropertySettingContainer::FontFamily>(ui->fontComboBox->currentIndex());
	c.fontSize = ui->sizeSpinBox->value();
	c.fontColor = ui->colorWidget->color();
	c.isBold = ui->boldButton->isChecked();
	c.isItalic = ui->italicButton->isChecked();
	c.isShadow = ui->shadowButton->isChecked();
	return c;
}

void vtkTextPropertySettingDialog::setSetting(const vtkTextPropertySettingContainer& setting)
{
	m_prefix = setting.prefix();
	ui->fontComboBox->setCurrentIndex(static_cast<int>(setting.fontFamily));
	ui->sizeSpinBox->setValue(setting.fontSize);
	ui->colorWidget->setColor(setting.fontColor);
	ui->boldButton->setChecked(setting.isBold);
	ui->italicButton->setChecked(setting.isItalic);
	ui->shadowButton->setChecked(setting.isShadow);
}

void vtkTextPropertySettingDialog::disableSize()
{
	ui->sizeSpinBox->clear();
	ui->sizeSpinBox->setDisabled(true);
}

void vtkTextPropertySettingDialog::disableColor()
{
	ui->colorWidget->setColor(QColor(Qt::gray));
	ui->colorWidget->setDisabled(true);
}
