#include "preprocessorgridcrosssectionwindow2_controller.h"
#include "preprocessorgridcrosssectionwindow2_edittablecontroller.h"
#include "preprocessorgridcrosssectionwindow2_impl.h"
#include "../public/preprocessorgridcrosssectionwindow2_graphicsview.h"
#include "ui_preprocessorgridcrosssectionwindow2_controller.h"

#include <guicore/pre/grid/structured2dgrid.h>

PreProcessorGridCrosssectionWindow2::Controller::Controller(Impl* impl, QWidget *parent) :
	QWidget(parent),
	m_impl {impl},
	ui(new Ui::PreProcessorGridCrosssectionWindow2_Controller)
{
	ui->setupUi(this);

	connect(ui->iRadioButton, &QRadioButton::clicked, this, &Controller::handleIClick);
	connect(ui->jRadioButton, &QRadioButton::clicked, this, &Controller::handleJClick);

	connect(ui->iRadioButton, &QRadioButton::clicked, this, &Controller::handleSettingChange);
	connect(ui->jRadioButton, &QRadioButton::clicked, this, &Controller::handleSettingChange);
	connect(ui->positionSlider, &SliderWithValue::valueChanged, [=](int){handleSettingChange();});
	connect(ui->previousRadioButton, &QRadioButton::clicked, this, &Controller::handleSettingChange);
	connect(ui->nextRadioButton, &QRadioButton::clicked, this, &Controller::handleSettingChange);

	connect(ui->snapshotButton, &QPushButton::clicked, [=](bool){emit snapshotClicked();});
	connect(ui->exportButton, &QPushButton::clicked, [=](bool){emit csvExportClicked();});
}

PreProcessorGridCrosssectionWindow2::Controller::~Controller()
{
	delete ui;
}

void PreProcessorGridCrosssectionWindow2::Controller::setTarget(Direction dir, int index)
{
	if (dir == Direction::I) {
		ui->iRadioButton->setChecked(true);
		handleIClick();
	} else {
		ui->jRadioButton->setChecked(true);
		handleJClick();
	}

	updateTargetRange();
	ui->positionSlider->setValue(index + 1);

	handleSettingChange();
}

PreProcessorGridCrosssectionWindow2::Direction PreProcessorGridCrosssectionWindow2::Controller::targetDirection() const
{
	if (ui->iRadioButton->isChecked()) {
		return Direction::I;
	} else {
		return Direction::J;
	}
}

int PreProcessorGridCrosssectionWindow2::Controller::targetIndex() const
{
	return ui->positionSlider->value() - 1;
}

int PreProcessorGridCrosssectionWindow2::Controller::maxIndex() const
{
	return ui->positionSlider->maximum() - 1;
}

PreProcessorGridCrosssectionWindow2::Controller::CellSide PreProcessorGridCrosssectionWindow2::Controller::cellSide() const
{
	if (ui->previousRadioButton->isChecked()) {
		return CellSide::Previous;
	} else if (ui->nextRadioButton->isChecked()) {
		return CellSide::Next;
	}
	// to avoid compiler warning
	return CellSide::Previous;
}

void PreProcessorGridCrosssectionWindow2::Controller::setCellSide(CellSide side)
{
	if (side == CellSide::Previous) {
		ui->previousRadioButton->setChecked(true);
	} else if (side == CellSide::Next) {
		ui->nextRadioButton->setChecked(true);
	}
}

void PreProcessorGridCrosssectionWindow2::Controller::handleIClick()
{
	ui->previousRadioButton->setText(tr("Upstream Side"));
	ui->nextRadioButton->setText(tr("Downstream Side"));

	updateTargetRange();
}

void PreProcessorGridCrosssectionWindow2::Controller::handleJClick()
{
	ui->previousRadioButton->setText(tr("Right Side"));
	ui->nextRadioButton->setText(tr("Left Side"));

	updateTargetRange();
}

void PreProcessorGridCrosssectionWindow2::Controller::handleSettingChange()
{
	m_impl->setTargetDirection(targetDirection());
	m_impl->m_window->update();

	if (m_impl->m_displaySetting.enableAutoRescale) {
		m_impl->m_window->cameraFit();
	}
}

void PreProcessorGridCrosssectionWindow2::Controller::updateTargetRange()
{
	auto grid = m_impl->grid();

	int dataCount = 0;
	if (ui->iRadioButton->isChecked()) {
		dataCount = grid->dimensionI();
	} else if (ui->jRadioButton->isChecked()) {
		dataCount = grid->dimensionJ();
	}
	ui->positionSlider->setRange(1, dataCount);
}
