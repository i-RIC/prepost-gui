#include "geodatariversurveycrosssectionslopepointeditdialog.h"
#include "geodatariversurveycrosssectionwindow.h"
#include "geodatariversurveycrosssectionwindowgraphicsview.h"
#include "ui_geodatariversurveycrosssectionslopepointeditdialog.h"

#include <misc/iricundostack.h>

#include <QMessageBox>

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

void GeoDataRiverSurveyCrosssectionSlopePointEditDialog::calculateLeftAndRightPoints(const GeoDataRiverCrosssection::AltitudeList& alist, Mode mode, const QPointF& point, int slope, QPointF* left, QPointF* right)
{
	double leftX = alist.begin()->position();
	double rightX = alist.rbegin()->position();

	if (mode == Mode::LeftAdd) {
		*left = QPointF(leftX, point.y());
		auto dy = (rightX - point.x()) / slope;
		*right = QPointF(rightX, point.y() - dy);
	} else if (mode == Mode::LeftSub) {
		*right = QPointF(rightX, point.y());
		auto dy = (point.x() - leftX) / slope;
		*left = QPointF(leftX, point.y() + dy);
	} else if (mode == Mode::RightAdd) {
		*right = QPointF(rightX, point.y());
		auto dy = (point.x() - leftX) / slope;
		*left = QPointF(leftX, point.y() - dy);
	} else if (mode == Mode::RightSub) {
		*left = QPointF(leftX, point.y());
		auto dy = (rightX - point.x()) / slope;
		*right = QPointF(rightX, point.y() + dy);
	}
}

void GeoDataRiverSurveyCrosssectionSlopePointEditDialog::accept()
{
	auto com = createCommand(false);
	if (com == nullptr) {
		QMessageBox::warning(this, tr("Warning"), tr("Please specify the setting so that the lines crosses the original cross-section."));
		return;
	}

	iRICUndoStack::instance().push(com);
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
	auto com = createCommand(true);
	if (com == nullptr) {
		QMessageBox::warning(this, tr("Warning"), tr("Please specify the setting so that the lines crosses the original cross-section."));
		return;
	}

	iRICUndoStack::instance().push(com);
}

GeoDataRiverSurveyCrosssectionWindow* GeoDataRiverSurveyCrosssectionSlopePointEditDialog::crosssectionWindow() const
{
	return dynamic_cast<GeoDataRiverSurveyCrosssectionWindow*> (parentWidget());
}

QUndoCommand* GeoDataRiverSurveyCrosssectionSlopePointEditDialog::createCommand(bool apply) const
{
	return nullptr;
}
