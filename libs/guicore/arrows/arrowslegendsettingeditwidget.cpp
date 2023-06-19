#include "arrowslegendsettingcontainer.h"
#include "arrowslegendsettingeditwidget.h"
#include "ui_arrowslegendsettingeditwidget.h"

ArrowsLegendSettingEditWidget::ArrowsLegendSettingEditWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ArrowsLegendSettingEditWidget)
{
	ui->setupUi(this);
}

ArrowsLegendSettingEditWidget::~ArrowsLegendSettingEditWidget()
{
	delete ui;
}

ArrowsLegendSettingContainer ArrowsLegendSettingEditWidget::setting() const
{
	ArrowsLegendSettingContainer ret;

	ret.visible = ui->visibleCheckBox->isChecked();
	ret.title = ui->titleEdit->text();
	ret.titleFont = ui->titleFontWidget->font();
	ret.lengthFormat = ui->lengthFormatEdit->text();
	ret.lengthFont = ui->lengthFontWidget->font();
	ret.titleColor = ui->titleColorWidget->color();
	ret.lengthColor = ui->lengthColorWidget->color();
	ret.backgroundColor = ui->backgroundColorWidget->color();
	ret.backgroundOpacity = ui->backgroundOpacityWidget->opacity();
	ret.showBorder = ui->showBorderCheckBox->isChecked();
	ret.imageSetting = ui->imageSettingWidget->setting();

	return ret;
}

void ArrowsLegendSettingEditWidget::setSetting(const ArrowsLegendSettingContainer& setting)
{
	ui->visibleCheckBox->setChecked(setting.visible);
	ui->titleEdit->setText(setting.title);
	ui->titleFontWidget->setFont(setting.titleFont);
	ui->lengthFormatEdit->setText(setting.lengthFormat);
	ui->lengthFontWidget->setFont(setting.lengthFont);
	ui->titleColorWidget->setColor(setting.titleColor);
	ui->lengthColorWidget->setColor(setting.lengthColor);
	ui->backgroundColorWidget->setColor(setting.backgroundColor);
	ui->backgroundOpacityWidget->setOpacity(setting.backgroundOpacity);
	ui->showBorderCheckBox->setChecked(setting.showBorder);
	ui->imageSettingWidget->setSetting(setting.imageSetting);
}

void ArrowsLegendSettingEditWidget::setImageSetting(const ImageSettingContainer& setting)
{
	ui->imageSettingWidget->setSetting(setting);
}

void ArrowsLegendSettingEditWidget::setTitle(const QString& title)
{
	ui->titleEdit->setText(title);
}
