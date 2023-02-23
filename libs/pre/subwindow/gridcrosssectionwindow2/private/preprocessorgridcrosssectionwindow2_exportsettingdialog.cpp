#include "preprocessorgridcrosssectionwindow2_exportsettingdialog.h"
#include "preprocessorgridcrosssectionwindow2_impl.h"
#include "../../../datamodel/preprocessorgriddataitem.h"
#include "ui_preprocessorgridcrosssectionwindow2_exportsettingdialog.h"

#include <guicore/pre/grid/structured2dgrid.h>
#include <misc/lastiodirectory.h>

PreProcessorGridCrosssectionWindow2::ExportSettingDialog::ExportSettingDialog(PreProcessorGridCrosssectionWindow2* window) :
	QDialog(window),
	m_window {window},
	ui(new Ui::PreProcessorGridCrosssectionWindow2_ExportSettingDialog)
{
	ui->setupUi(this);
	connect(ui->currentRadioButton, &QRadioButton::clicked, [=] (bool) {handleCurrentClicked();});
	connect(ui->allRadioButton, &QRadioButton::clicked, [=] (bool) {handleAllClicked();});

	auto grid = dynamic_cast<Structured2DGrid*>(window->impl->m_gridDataItem->grid());

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

PreProcessorGridCrosssectionWindow2::ExportSettingDialog::~ExportSettingDialog()
{
	delete ui;
}

QString PreProcessorGridCrosssectionWindow2::ExportSettingDialog::dirName() const
{
	return ui->directoryEditWidget->dirname();
}

void PreProcessorGridCrosssectionWindow2::ExportSettingDialog::setDirName(const QString& dirName)
{
	return ui->directoryEditWidget->setDirname(dirName);
}

QString PreProcessorGridCrosssectionWindow2::ExportSettingDialog::prefix() const
{
	return ui->prefixEdit->text();
}

void PreProcessorGridCrosssectionWindow2::ExportSettingDialog::setPrefix(const QString& prefix)
{
	ui->prefixEdit->setText(prefix);
}

PreProcessorGridCrosssectionWindow2::ExportSettingDialog::CrossSectionMode PreProcessorGridCrosssectionWindow2::ExportSettingDialog::mode() const
{
	if (ui->currentRadioButton->isChecked()) {
		return CrossSectionMode::Current;
	} else if (ui->allRadioButton->isChecked()) {
		return CrossSectionMode::All;
	} else if (ui->currentRadioButton->isChecked()) {
		return CrossSectionMode::Custom;
	}
}

void PreProcessorGridCrosssectionWindow2::ExportSettingDialog::setMode(CrossSectionMode mode)
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

int PreProcessorGridCrosssectionWindow2::ExportSettingDialog::min() const
{
	return ui->minSlider->value() - 1;
}

void PreProcessorGridCrosssectionWindow2::ExportSettingDialog::setMin(int min)
{
	ui->minSlider->setValue(min + 1);
}

int PreProcessorGridCrosssectionWindow2::ExportSettingDialog::max() const
{
	return ui->maxSlider->value() - 1;
}

void PreProcessorGridCrosssectionWindow2::ExportSettingDialog::setMax(int max)
{
	ui->maxSlider->setValue(max + 1);
}

int PreProcessorGridCrosssectionWindow2::ExportSettingDialog::skipRate() const
{
	return ui->skipRateSpinBox->value();
}

void PreProcessorGridCrosssectionWindow2::ExportSettingDialog::setSkipRate(int skipRate)
{
	ui->skipRateSpinBox->setValue(skipRate);
}

void PreProcessorGridCrosssectionWindow2::ExportSettingDialog::accept()
{
	LastIODirectory::set(dirName());
	QDialog::accept();
}

void PreProcessorGridCrosssectionWindow2::ExportSettingDialog::handleCurrentClicked()
{
	int index = m_window->targetIndex();
	ui->minSlider->setValue(index + 1);
	ui->maxSlider->setValue(index + 1);
}

void PreProcessorGridCrosssectionWindow2::ExportSettingDialog::handleAllClicked()
{
	ui->minSlider->setValue(ui->minSlider->minimum());
	ui->maxSlider->setValue(ui->maxSlider->maximum());
}
