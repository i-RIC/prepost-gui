#include "ui_geodatariverpathpointmovedialog.h"

#include "geodatariverpathpoint.h"
#include "geodatariverpathpointmovedialog.h"
#include "geodatariversurvey.h"
#include "geodatariversurveybackgroundgridcreatethread.h"
#include "private/geodatariversurvey_moveriverpathpointcommand.h"

#include <misc/iricundostack.h>

#include <QUndoCommand>

GeoDataRiverPathPointMoveDialog::GeoDataRiverPathPointMoveDialog(GeoDataRiverSurvey* rs, QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GeoDataRiverPathPointMoveDialog)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);
	m_rs = rs;
	m_applyed = false;
	ui->offsetModeRadioButton->setChecked(true);
	ui->offsetX->setValue(0);
	ui->offsetY->setValue(0);
	ui->coordX->setDisabled(true);
	ui->coordY->setDisabled(true);
	connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(handleButtonClick(QAbstractButton*)));

	connect(ui->coordX, SIGNAL(valueChanged(double)), this, SLOT(centerChange()));
	connect(ui->coordY, SIGNAL(valueChanged(double)), this, SLOT(centerChange()));
	connect(ui->offsetX, SIGNAL(valueChanged(double)), this, SLOT(offsetChange()));
	connect(ui->offsetY, SIGNAL(valueChanged(double)), this, SLOT(offsetChange()));
}

GeoDataRiverPathPointMoveDialog::~GeoDataRiverPathPointMoveDialog()
{
	delete ui;
}


void GeoDataRiverPathPointMoveDialog::setCurrentCenter(const QPointF &current)
{
	m_currentCenter = current;
	ui->coordX->setValue(current.x());
	ui->coordY->setValue(current.y());
}

void GeoDataRiverPathPointMoveDialog::setSingleSelection(bool single)
{
	ui->centerPointModeRadioButton->setEnabled(single);
	if (! single) {
		ui->coordX->clear();
		ui->coordY->clear();
	}
}

void GeoDataRiverPathPointMoveDialog::accept()
{
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
	}
	QPointF offset(ui->offsetX->value(), ui->offsetY->value());
	iRICUndoStack::instance().push(new GeoDataRiverSurvey::MoveRiverPathPointCommand(false, offset, m_rs));
	QDialog::accept();
}

void GeoDataRiverPathPointMoveDialog::reject()
{
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
		m_rs->headPoint()->updateRiverShapeInterpolators();
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
	}
	QDialog::reject();
}

void GeoDataRiverPathPointMoveDialog::doReset()
{
	ui->offsetModeRadioButton->setChecked(true);
	ui->offsetX->setValue(0);
	ui->offsetY->setValue(0);
	if (ui->centerPointModeRadioButton->isEnabled()) {
		ui->coordX->setValue(m_currentCenter.x());
		ui->coordY->setValue(m_currentCenter.y());
	}
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
		m_rs->headPoint()->updateRiverShapeInterpolators();
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
	}
	m_applyed = false;
}

void GeoDataRiverPathPointMoveDialog::apply()
{
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
	}
	QPointF offset(ui->offsetX->value(), ui->offsetY->value());
	iRICUndoStack::instance().push(new GeoDataRiverSurvey::MoveRiverPathPointCommand(true, offset, m_rs));
	m_applyed = true;
}

void GeoDataRiverPathPointMoveDialog::offsetChange()
{
	if (ui->offsetModeRadioButton->isChecked() && ui->centerPointModeRadioButton->isEnabled()) {
		double offsetx = ui->offsetX->value();
		double offsety = ui->offsetY->value();
		ui->coordX->setValue(m_currentCenter.x() + offsetx);
		ui->coordY->setValue(m_currentCenter.y() + offsety);
	}
}

void GeoDataRiverPathPointMoveDialog::centerChange()
{
	if (ui->centerPointModeRadioButton->isChecked()) {
		ui->offsetX->setValue(ui->coordX->value() - m_currentCenter.x());
		ui->offsetY->setValue(ui->coordY->value() - m_currentCenter.y());
	}
}

void GeoDataRiverPathPointMoveDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	} else if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ResetRole) {
		doReset();
	}
}
