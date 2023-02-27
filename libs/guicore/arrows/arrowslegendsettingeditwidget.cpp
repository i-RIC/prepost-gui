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

	if (ui->visibilityWhenSelectedRadioButton->isChecked()) {
		ret.visibilityMode = ArrowsLegendSettingContainer::VisibilityMode::WhenSelected;
	} else if (ui->visibilityAlwaysRadioButton->isChecked()) {
		ret.visibilityMode = ArrowsLegendSettingContainer::VisibilityMode::Always;
	} else if (ui->visibilityNeverRadioButton->isChecked()) {
		ret.visibilityMode = ArrowsLegendSettingContainer::VisibilityMode::Never;
	}
	ret.title = ui->titleEdit->text();
	ret.titleFont = ui->titleFontWidget->font();
	ret.lengthFormat = ui->lengthFormatEdit->text();
	ret.lengthFont = ui->lengthFontWidget->font();
	ret.titleColor = ui->titleColorWidget->color();
	ret.lengthColor = ui->lengthColorWidget->color();
	ret.backgroundColor = ui->backgroundColorWidget->color();
	ret.backgroundOpacity = ui->backgroundOpacitySlider->opacityPercent();
	ret.imageSetting = ui->imageSettingWidget->setting();

	return ret;
}

void ArrowsLegendSettingEditWidget::setSetting(const ArrowsLegendSettingContainer& setting)
{
	if (setting.visibilityMode == ArrowsLegendSettingContainer::VisibilityMode::WhenSelected) {
		ui->visibilityWhenSelectedRadioButton->setChecked(true);
	} else if (setting.visibilityMode == ArrowsLegendSettingContainer::VisibilityMode::Always) {
		ui->visibilityAlwaysRadioButton->setChecked(true);
	} else if (setting.visibilityMode == ArrowsLegendSettingContainer::VisibilityMode::Never) {
		ui->visibilityNeverRadioButton->setChecked(true);
	}
	ui->titleEdit->setText(setting.title);
	ui->titleFontWidget->setFont(setting.titleFont);
	ui->lengthFormatEdit->setText(setting.lengthFormat);
	ui->lengthFontWidget->setFont(setting.lengthFont);
	ui->titleColorWidget->setColor(setting.titleColor);
	ui->lengthColorWidget->setColor(setting.lengthColor);
	ui->backgroundColorWidget->setColor(setting.backgroundColor);
	ui->backgroundOpacitySlider->setOpacityPercent(setting.backgroundOpacity);
	ui->imageSettingWidget->setSetting(setting.imageSetting);
}

void ArrowsLegendSettingEditWidget::setTitle(const QString& title)
{
	ui->titleEdit->setText(title);
}
