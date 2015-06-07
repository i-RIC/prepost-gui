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
	c.setFontFamily(static_cast<vtkTextPropertySettingContainer::FontFamily>(ui->fontComboBox->currentIndex()));
	c.setFontSize(ui->sizeSpinBox->value());
	c.setFontColor(ui->colorWidget->color());
	c.setBold(ui->boldButton->isChecked());
	c.setItalic(ui->italicButton->isChecked());
	c.setShadow(ui->shadowButton->isChecked());
	return c;
}

void vtkTextPropertySettingDialog::setSetting(const vtkTextPropertySettingContainer& setting)
{
	m_prefix = setting.prefix();
	ui->fontComboBox->setCurrentIndex(static_cast<int>(setting.fontFamily()));
	ui->sizeSpinBox->setValue(setting.fontSize());
	ui->colorWidget->setColor(setting.fontColor());
	ui->boldButton->setChecked(setting.isBold());
	ui->italicButton->setChecked(setting.isItalic());
	ui->shadowButton->setChecked(setting.isShadow());
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
