#include "ui_vtktextpropertysettingwidget.h"

#include "vtktextpropertysettingcontainer.h"
#include "vtktextpropertysettingwidget.h"

vtkTextPropertySettingWidget::vtkTextPropertySettingWidget(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::vtkTextPropertySettingWidget)
{
	ui->setupUi(this);
}

vtkTextPropertySettingWidget::~vtkTextPropertySettingWidget()
{
	delete ui;
}

vtkTextPropertySettingContainer vtkTextPropertySettingWidget::setting() const
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

void vtkTextPropertySettingWidget::setSetting(const vtkTextPropertySettingContainer& setting)
{
	m_prefix = setting.prefix();
	ui->fontComboBox->setCurrentIndex(static_cast<int>(setting.fontFamily));
	ui->sizeSpinBox->setValue(setting.fontSize);
	ui->colorWidget->setColor(setting.fontColor);
	ui->boldButton->setChecked(setting.isBold);
	ui->italicButton->setChecked(setting.isItalic);
	ui->shadowButton->setChecked(setting.isShadow);
}

void vtkTextPropertySettingWidget::disableSize()
{
	ui->sizeSpinBox->clear();
	ui->sizeSpinBox->setDisabled(true);
}

void vtkTextPropertySettingWidget::disableColor()
{
	ui->colorWidget->setColor(QColor(Qt::gray));
	ui->colorWidget->setDisabled(true);
}

void vtkTextPropertySettingWidget::hideColor()
{
	ui->colorLabel->hide();
	ui->colorWidget->hide();
	updateGeometry();
}
