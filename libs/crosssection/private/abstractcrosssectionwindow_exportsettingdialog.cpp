#include "abstractcrosssectionwindow_exportsettingdialog.h"
#include "abstractcrosssectionwindow_impl.h"
#include "ui_abstractcrosssectionwindow_exportsettingdialog.h"

#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <misc/lastiodirectory.h>

AbstractCrosssectionWindow::ExportSettingDialog::ExportSettingDialog(AbstractCrosssectionWindow* window) :
	QDialog(window),
	m_window {window},
	ui(new Ui::AbstractCrosssectionWindow_ExportSettingDialog)
{
	ui->setupUi(this);
	connect(ui->currentRadioButton, &QRadioButton::clicked, [=] (bool) {handleCurrentClicked();});
	connect(ui->allRadioButton, &QRadioButton::clicked, [=] (bool) {handleAllClicked();});

	auto grid = window->grid();

	if (window->targetDirection() == Direction::I) {
		int iDim = grid->dimensionI();
		ui->minSlider->setRange(1, iDim);
		ui->maxSlider->setRange(1, iDim);
	} else {
		int jDim = grid->dimensionJ();
		ui->minSlider->setRange(1, jDim);
		ui->maxSlider->setRange(1, jDim);
	}

	handleCurrentClicked();
}

AbstractCrosssectionWindow::ExportSettingDialog::~ExportSettingDialog()
{
	delete ui;
}

QString AbstractCrosssectionWindow::ExportSettingDialog::dirName() const
{
	return ui->directoryEditWidget->dirname();
}

void AbstractCrosssectionWindow::ExportSettingDialog::setDirName(const QString& dirName)
{
	return ui->directoryEditWidget->setDirname(dirName);
}

QString AbstractCrosssectionWindow::ExportSettingDialog::prefix() const
{
	return ui->prefixEdit->text();
}

void AbstractCrosssectionWindow::ExportSettingDialog::setPrefix(const QString& prefix)
{
	ui->prefixEdit->setText(prefix);
}

AbstractCrosssectionWindow::ExportSettingDialog::CrossSectionMode AbstractCrosssectionWindow::ExportSettingDialog::mode() const
{
	if (ui->currentRadioButton->isChecked()) {
		return CrossSectionMode::Current;
	} else if (ui->allRadioButton->isChecked()) {
		return CrossSectionMode::All;
	} else if (ui->currentRadioButton->isChecked()) {
		return CrossSectionMode::Custom;
	}
	return CrossSectionMode::All;
}

void AbstractCrosssectionWindow::ExportSettingDialog::setMode(CrossSectionMode mode)
{
	if (mode == CrossSectionMode::Current) {
		ui->currentRadioButton->setChecked(true);
		handleCurrentClicked();
	} else if (mode == CrossSectionMode::All) {
		ui->allRadioButton->setChecked(true);
		handleAllClicked();
	} else if (mode == CrossSectionMode::Custom) {
		ui->customRadioButton->setChecked(true);
	}
}

int AbstractCrosssectionWindow::ExportSettingDialog::min() const
{
	return ui->minSlider->value() - 1;
}

void AbstractCrosssectionWindow::ExportSettingDialog::setMin(int min)
{
	ui->minSlider->setValue(min + 1);
}

int AbstractCrosssectionWindow::ExportSettingDialog::max() const
{
	return ui->maxSlider->value() - 1;
}

void AbstractCrosssectionWindow::ExportSettingDialog::setMax(int max)
{
	ui->maxSlider->setValue(max + 1);
}

int AbstractCrosssectionWindow::ExportSettingDialog::skipRate() const
{
	return ui->skipRateSpinBox->value();
}

void AbstractCrosssectionWindow::ExportSettingDialog::setSkipRate(int skipRate)
{
	ui->skipRateSpinBox->setValue(skipRate);
}

void AbstractCrosssectionWindow::ExportSettingDialog::accept()
{
	LastIODirectory::set(dirName());
	QDialog::accept();
}

void AbstractCrosssectionWindow::ExportSettingDialog::handleCurrentClicked()
{
	int index = m_window->targetIndex();
	ui->minSlider->setValue(index + 1);
	ui->maxSlider->setValue(index + 1);
}

void AbstractCrosssectionWindow::ExportSettingDialog::handleAllClicked()
{
	ui->minSlider->setValue(ui->minSlider->minimum());
	ui->maxSlider->setValue(ui->maxSlider->maximum());
}
