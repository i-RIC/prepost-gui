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
	auto s = m_setting;
	s.fontFamily = static_cast<vtkTextPropertySettingContainer::FontFamily>(ui->fontComboBox->currentIndex());
	s.fontSize = ui->sizeSpinBox->value();
	s.fontColor = ui->colorWidget->color();
	s.isBold = ui->boldButton->isChecked();
	s.isItalic = ui->italicButton->isChecked();
	s.isShadow = ui->shadowButton->isChecked();
	return s;
}

void vtkTextPropertySettingWidget::setSetting(const vtkTextPropertySettingContainer& setting)
{
	m_setting = setting;

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

void vtkTextPropertySettingWidget::hideSize()
{
	ui->sizeLabel->hide();
	ui->sizeSpinBox->hide();
	updateGeometry();
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
