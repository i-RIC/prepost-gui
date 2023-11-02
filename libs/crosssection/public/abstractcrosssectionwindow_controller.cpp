#include "../abstractcrosssectionwindow.h"
#include "../private/abstractcrosssectionwindow_impl.h"
#include "abstractcrosssectionwindow_controller.h"
#include "abstractcrosssectionwindow_graphicsview.h"
#include "ui_abstractcrosssectionwindow_controller.h"

#include <guicore/grid/v4structured2dgrid.h>

AbstractCrosssectionWindow::Controller::Controller(AbstractCrosssectionWindow* window, QWidget *parent) :
	QWidget(parent),
	m_window {window},
	ui(new Ui::AbstractCrosssectionWindow_Controller)
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

AbstractCrosssectionWindow::Controller::~Controller()
{
	delete ui;
}

void AbstractCrosssectionWindow::Controller::setTarget(Direction dir, int index)
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

AbstractCrosssectionWindow::Direction AbstractCrosssectionWindow::Controller::targetDirection() const
{
	if (ui->iRadioButton->isChecked()) {
		return Direction::I;
	} else {
		return Direction::J;
	}
}

int AbstractCrosssectionWindow::Controller::targetIndex() const
{
	return ui->positionSlider->value() - 1;
}

int AbstractCrosssectionWindow::Controller::maxIndex() const
{
	return ui->positionSlider->maximum() - 1;
}

AbstractCrosssectionWindow::Controller::CellSide AbstractCrosssectionWindow::Controller::cellSide() const
{
	if (ui->previousRadioButton->isChecked()) {
		return CellSide::Previous;
	} else if (ui->nextRadioButton->isChecked()) {
		return CellSide::Next;
	}
	// to avoid compiler warning
	return CellSide::Previous;
}

void AbstractCrosssectionWindow::Controller::setCellSide(CellSide side)
{
	if (side == CellSide::Previous) {
		ui->previousRadioButton->setChecked(true);
	} else if (side == CellSide::Next) {
		ui->nextRadioButton->setChecked(true);
	}
}

void AbstractCrosssectionWindow::Controller::handleIClick()
{
	ui->previousRadioButton->setText(tr("Upstream Side"));
	ui->nextRadioButton->setText(tr("Downstream Side"));

	updateTargetRange();
}

void AbstractCrosssectionWindow::Controller::handleJClick()
{
	ui->previousRadioButton->setText(tr("Right Side"));
	ui->nextRadioButton->setText(tr("Left Side"));

	updateTargetRange();
}

void AbstractCrosssectionWindow::Controller::handleSettingChange()
{
	m_window->impl->setTargetDirection(targetDirection());

	m_window->handleSettingChange();
}

void AbstractCrosssectionWindow::Controller::updateTargetRange()
{
	auto grid = m_window->grid();
	if (grid == nullptr) {return;}

	int dataCount = 0;
	if (ui->iRadioButton->isChecked()) {
		dataCount = grid->dimensionI();
	} else if (ui->jRadioButton->isChecked()) {
		dataCount = grid->dimensionJ();
	}
	ui->positionSlider->setRange(1, dataCount);
}
