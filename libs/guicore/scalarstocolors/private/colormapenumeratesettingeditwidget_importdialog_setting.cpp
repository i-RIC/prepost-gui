#include "colormapenumeratesettingeditwidget_importdialog_setting.h"
#include "ui_colormapenumeratesettingeditwidget_importdialog.h"

ColorMapEnumerateSettingEditWidget::ImportDialog::Setting ColorMapEnumerateSettingEditWidget::ImportDialog::Setting::instance;

void ColorMapEnumerateSettingEditWidget::ImportDialog::Setting::get(ImportDialog* dialog)
{
	dialog->ui->directionCheckBox->setChecked(instance.direction);
	dialog->ui->titleGroupCheckBox->setChecked(instance.titleGroup);
	dialog->ui->titleCheckBox->setChecked(instance.title);
	dialog->ui->titleFontSettingCheckBox->setChecked(instance.titleFont);
	dialog->ui->labelDisplayGroupCheckBox->setChecked(instance.labelDisplayFont);
	dialog->ui->colorGroupCheckBox->setChecked(instance.colorGroup);
	dialog->ui->colorTitleCheckBox->setChecked(instance.colorTitle);
	dialog->ui->colorLabelCheckBox->setChecked(instance.colorLabel);
	dialog->ui->colorBackgroundCheckBox->setChecked(instance.colorBackground);
	dialog->ui->sizeAndPositionGroupCheckBox->setChecked(instance.sizeAndPosition);
	dialog->ui->positionCheckBox->setChecked(instance.position);
	dialog->ui->sizeCheckBox->setChecked(instance.size);
	dialog->ui->barWidthCheckBox->setChecked(instance.barWidth);
	dialog->ui->alignmentCheckBox->setChecked(instance.alignment);
}

void ColorMapEnumerateSettingEditWidget::ImportDialog::Setting::set(ImportDialog* dialog)
{
	instance.direction = dialog->ui->directionCheckBox->isChecked();
	instance.titleGroup = dialog->ui->titleGroupCheckBox->isChecked();
	instance.title = dialog->ui->titleCheckBox->isChecked();
	instance.titleFont = dialog->ui->titleFontSettingCheckBox->isChecked();
	instance.labelDisplayFont = dialog->ui->labelDisplayGroupCheckBox->isChecked();
	instance.colorGroup = dialog->ui->colorGroupCheckBox->isChecked();
	instance.colorTitle = dialog->ui->colorTitleCheckBox->isChecked();
	instance.colorLabel = dialog->ui->colorLabelCheckBox->isChecked();
	instance.colorBackground = dialog->ui->colorBackgroundCheckBox->isChecked();
	instance.sizeAndPosition = dialog->ui->sizeAndPositionGroupCheckBox->isChecked();
	instance.position = dialog->ui->positionCheckBox->isChecked();
	instance.size = dialog->ui->sizeCheckBox->isChecked();
	instance.barWidth = dialog->ui->barWidthCheckBox->isChecked();
	instance.alignment = dialog->ui->alignmentCheckBox->isChecked();
}

ColorMapEnumerateSettingEditWidget::ImportDialog::Setting::Setting() :
	direction {true},
	titleGroup {false},
	title {false},
	titleFont {true},
	labelDisplayFont {true},
	colorGroup {true},
	colorTitle {false},
	colorLabel {false},
	colorBackground {false},
	sizeAndPosition {true},
	position {false},
	size {false},
	barWidth {true},
	alignment {true}
{}
