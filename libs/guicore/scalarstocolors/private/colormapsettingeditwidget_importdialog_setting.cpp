#include "colormapsettingeditwidget_importdialog_setting.h"
#include "ui_colormapsettingeditwidget_importdialog.h"

ColorMapSettingEditWidget::ImportDialog::Setting ColorMapSettingEditWidget::ImportDialog::Setting::instance;

void ColorMapSettingEditWidget::ImportDialog::Setting::get(ImportDialog* dialog)
{
	dialog->ui->directionCheckBox->setChecked(instance.direction);
	dialog->ui->titleGroupCheckBox->setChecked(instance.titleGroup);
	dialog->ui->titleCheckBox->setChecked(instance.title);
	dialog->ui->titleFontSettingCheckBox->setChecked(instance.titleFont);
	dialog->ui->labelDisplayGroupCheckBox->setChecked(instance.labelDisplayGroup);
	dialog->ui->labelFormatCheckBox->setChecked(instance.labelFormat);
	dialog->ui->numLabelsCheckBox->setChecked(instance.labelNumLabels);
	dialog->ui->labelFontSettingCheckBox->setChecked(instance.labelFont);
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

void ColorMapSettingEditWidget::ImportDialog::Setting::set(ImportDialog* dialog)
{
	instance.direction = dialog->ui->directionCheckBox->isChecked();
	instance.titleGroup = dialog->ui->titleGroupCheckBox->isChecked();
	instance.title = dialog->ui->titleCheckBox->isChecked();
	instance.titleFont = dialog->ui->titleFontSettingCheckBox->isChecked();
	instance.labelDisplayGroup = dialog->ui->labelDisplayGroupCheckBox->isChecked();
	instance.labelFormat = dialog->ui->labelFormatCheckBox->isChecked();
	instance.labelNumLabels = dialog->ui->numLabelsCheckBox->isChecked();
	instance.labelFont = dialog->ui->labelFontSettingCheckBox->isChecked();
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

ColorMapSettingEditWidget::ImportDialog::Setting::Setting() :
	direction {true},
	titleGroup {false},
	title {false},
	titleFont {true},
	labelDisplayGroup {true},
	labelFormat {false},
	labelNumLabels {false},
	labelFont {false},
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
