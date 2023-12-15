#include "abstractcrosssectionwindow_displaysettingcontainer.h"
#include "abstractcrosssectionwindow_displaysettingdialog.h"
#include "abstractcrosssectionwindow_impl.h"
#include "ui_abstractcrosssectionwindow_displaysettingdialog.h"

#include <misc/iricundostack.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

AbstractCrosssectionWindow::DisplaySettingDialog::DisplaySettingDialog(AbstractCrosssectionWindow* window) :
	QDialog {window},
	m_setting {window->impl->m_displaySetting},
	m_window {window},
	ui(new Ui::AbstractCrosssectionWindow_DisplaySettingDialog)
{
	ui->setupUi(this);
	connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &DisplaySettingDialog::handleButtonClick);

	setSetting(window->impl->m_displaySetting);
}

AbstractCrosssectionWindow::DisplaySettingDialog::~DisplaySettingDialog()
{
	delete ui;
}

QUndoCommand* AbstractCrosssectionWindow::DisplaySettingDialog::createModifyCommand(bool apply)
{
	return new ValueModifyCommmand<DisplaySettingContainer> (iRIC::generateCommandId("AbstractCrosssectionWindow::DisplaySettingDialog::Modify"), apply, setting(), &m_window->impl->m_displaySetting);
}

void AbstractCrosssectionWindow::DisplaySettingDialog::accept()
{
	m_window->pushUpdateGraphicsViewCommand(createModifyCommand(false));

	QDialog::accept();
}
void AbstractCrosssectionWindow::DisplaySettingDialog::reject()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
	}

	QDialog::reject();
}

void AbstractCrosssectionWindow::DisplaySettingDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

void AbstractCrosssectionWindow::DisplaySettingDialog::apply()
{
	m_window->pushUpdateGraphicsViewCommand(createModifyCommand(true));
}

AbstractCrosssectionWindow::DisplaySettingContainer AbstractCrosssectionWindow::DisplaySettingDialog::setting() const
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

void AbstractCrosssectionWindow::DisplaySettingDialog::setSetting(const DisplaySettingContainer& setting)
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
