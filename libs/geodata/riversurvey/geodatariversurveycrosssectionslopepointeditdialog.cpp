#include "geodatariversurveycrosssectionslopepointeditdialog.h"
#include "geodatariversurveycrosssectionwindow.h"
#include "geodatariversurveycrosssectionwindowgraphicsview.h"
#include "ui_geodatariversurveycrosssectionslopepointeditdialog.h"

GeoDataRiverSurveyCrosssectionSlopePointEditDialog::GeoDataRiverSurveyCrosssectionSlopePointEditDialog(GeoDataRiverSurveyCrosssectionWindow *parent) :
	QDialog(parent),
	ui(new Ui::GeoDataRiverSurveyCrosssectionSlopePointEditDialog)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);

	connect<void (QSpinBox::*)(int)>(ui->slopeSpinBox, &QSpinBox::valueChanged, this, &GeoDataRiverSurveyCrosssectionSlopePointEditDialog::handleSlopeEdit);
	connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &GeoDataRiverSurveyCrosssectionSlopePointEditDialog::handleButtonClick);
}

GeoDataRiverSurveyCrosssectionSlopePointEditDialog::~GeoDataRiverSurveyCrosssectionSlopePointEditDialog()
{
	delete ui;
}

void GeoDataRiverSurveyCrosssectionSlopePointEditDialog::setMode(Mode mode)
{
	m_mode = mode;
	if (mode == Mode::LeftAdd)  {
		ui->modeValueLabel->setText(tr("Left Add"));
	} else if (mode == Mode::LeftSub) {
		ui->modeValueLabel->setText(tr("Left Sub"));
	} else if (mode == Mode::RightAdd) {
		ui->modeValueLabel->setText(tr("Right Add"));
	} else if (mode == Mode::RightSub) {
		ui->modeValueLabel->setText(tr("Right Sub"));
	}
}

int GeoDataRiverSurveyCrosssectionSlopePointEditDialog::slope() const
{
	return ui->slopeSpinBox->value();
}

void GeoDataRiverSurveyCrosssectionSlopePointEditDialog::setSlope(int slope)
{
	ui->slopeSpinBox->setValue(slope);
}

void GeoDataRiverSurveyCrosssectionSlopePointEditDialog::accept()
{
	QDialog::accept();
}

void GeoDataRiverSurveyCrosssectionSlopePointEditDialog::reject()
{
	QDialog::reject();
}

void GeoDataRiverSurveyCrosssectionSlopePointEditDialog::setPoint(const QPointF& point)
{
	if (! ui->clickRadioButton->isChecked()) {return;}

	ui->positionXEdit->setValue(point.x());
	ui->positionYEdit->setValue(point.y());

	apply();
}

void GeoDataRiverSurveyCrosssectionSlopePointEditDialog::handleSlopeEdit(int slope)
{
	QPointF point(ui->positionXEdit->value(), ui->positionYEdit->value());
	crosssectionWindow()->graphicsView()->setSlopePointEditModeSetting(point, slope);
}

void GeoDataRiverSurveyCrosssectionSlopePointEditDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

void GeoDataRiverSurveyCrosssectionSlopePointEditDialog::apply()
{
}

GeoDataRiverSurveyCrosssectionWindow* GeoDataRiverSurveyCrosssectionSlopePointEditDialog::crosssectionWindow() const
{
	return dynamic_cast<GeoDataRiverSurveyCrosssectionWindow*> (parentWidget());
}
