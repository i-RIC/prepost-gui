#include "preprocessorgridcrosssectionwindow2_displaysettingcontainer.h"
#include "preprocessorgridcrosssectionwindow2_displaysettingdialog.h"
#include "preprocessorgridcrosssectionwindow2_impl.h"
#include "ui_preprocessorgridcrosssectionwindow2_displaysettingdialog.h"

#include <misc/iricundostack.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

PreProcessorGridCrosssectionWindow2::DisplaySettingDialog::DisplaySettingDialog(PreProcessorGridCrosssectionWindow2* window) :
	QDialog {window},
	m_setting {window->impl->m_displaySetting},
	m_window {window},
	ui(new Ui::PreProcessorGridCrosssectionWindow2_DisplaySettingDialog)
{
	ui->setupUi(this);
	connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &DisplaySettingDialog::handleButtonClick);

	setSetting(window->impl->m_displaySetting);
}

PreProcessorGridCrosssectionWindow2::DisplaySettingDialog::~DisplaySettingDialog()
{
	delete ui;
}

QUndoCommand* PreProcessorGridCrosssectionWindow2::DisplaySettingDialog::createModifyCommand(bool apply)
{
	return new ValueModifyCommmand<DisplaySettingContainer> (iRIC::generateCommandId("PreProcessorGridCrosssectionWindow2::DisplaySettingDialog::Modify"), apply, setting(), &m_window->impl->m_displaySetting);
}

void PreProcessorGridCrosssectionWindow2::DisplaySettingDialog::accept()
{
	m_window->pushUpdateGraphicsViewCommand(createModifyCommand(false));

	QDialog::accept();
}
void PreProcessorGridCrosssectionWindow2::DisplaySettingDialog::reject()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
	}

	QDialog::reject();
}

void PreProcessorGridCrosssectionWindow2::DisplaySettingDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

void PreProcessorGridCrosssectionWindow2::DisplaySettingDialog::apply()
{
	m_window->pushUpdateGraphicsViewCommand(createModifyCommand(true));
}

PreProcessorGridCrosssectionWindow2::DisplaySettingContainer PreProcessorGridCrosssectionWindow2::DisplaySettingDialog::setting() const
{
	DisplaySettingContainer ret = m_window->impl->m_displaySetting;

	if (ui->bgLinesRadioButton->isChecked()) {
		ret.bgGridType = DisplaySettingContainer::BackgroundGridType::Lines;
	} else {
		ret.bgGridType = DisplaySettingContainer::BackgroundGridType::Dots;
	}
	ret.bgHScaleAuto = ui->horizontalAutoCheckBox->isChecked();
	ret.bgHScaleInterval = ui->horizontalScaleSpinBox->value();
	ret.bgHSubScaleInterval = ui->horizontalSubScaleSpinBox->value();

	ret.bgVScaleAuto = ui->verticalAutoCheckBox->isChecked();
	ret.bgVScaleInterval = ui->verticalScaleSpinBox->value();
	ret.bgVSubScaleInterval = ui->verticalSubScaleSpinBox->value();

	ret.bgGridColor = ui->gridColorWidget->color();

	ret.distanceMarkersColor = ui->distanceMarkersColorWidget->color();
	ret.distanceMarkersFont = ui->distanceMarkersFontWidget->font();

	ret.lbBankMarkersColor = ui->lbBankMakersColorWidget->color();
	ret.lbBankMarkersFont = ui->lbBankMakersFontWidget->font();

	ret.aspectRatioColor = ui->aspectRatioColorWidget->color();
	ret.aspectRatioFont = ui->aspectRatioFontWidget->font();

	return ret;
}

void PreProcessorGridCrosssectionWindow2::DisplaySettingDialog::setSetting(const DisplaySettingContainer& setting)
{
	if (setting.bgGridType == DisplaySettingContainer::BackgroundGridType::Lines) {
		ui->bgLinesRadioButton->setChecked(true);
	} else {
		ui->bgDotsRadioButton->setChecked(true);
	}
	ui->horizontalAutoCheckBox->setChecked(setting.bgHScaleAuto);
	ui->horizontalScaleSpinBox->setValue(setting.bgHScaleInterval);
	ui->horizontalSubScaleSpinBox->setValue(setting.bgHSubScaleInterval);

	ui->verticalAutoCheckBox->setChecked(setting.bgVScaleAuto);
	ui->verticalScaleSpinBox->setValue(setting.bgVScaleInterval);
	ui->verticalSubScaleSpinBox->setValue(setting.bgVSubScaleInterval);

	ui->gridColorWidget->setColor(setting.bgGridColor);

	ui->distanceMarkersColorWidget->setColor(setting.distanceMarkersColor);
	ui->distanceMarkersFontWidget->setFont(setting.distanceMarkersFont);

	ui->lbBankMakersColorWidget->setColor(setting.lbBankMarkersColor);
	ui->lbBankMakersFontWidget->setFont(setting.lbBankMarkersFont);

	ui->aspectRatioColorWidget->setColor(setting.aspectRatioColor);
	ui->aspectRatioFontWidget->setFont(setting.aspectRatioFont);
}
