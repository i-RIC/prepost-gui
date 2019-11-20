#include "ui_geodatariverpathpointshiftdialog.h"

#include "geodatariverpathpoint.h"
#include "geodatariverpathpointshiftdialog.h"
#include "geodatariversurvey.h"
#include "geodatariversurveybackgroundgridcreatethread.h"
#include "private/geodatariversurvey_shiftriverpathcentercommand.h"

#include <misc/iricundostack.h>

#include <QPushButton>
#include <QUndoCommand>

GeoDataRiverPathPointShiftDialog::GeoDataRiverPathPointShiftDialog(GeoDataRiverSurvey* rs, QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GeoDataRiverPathPointShiftDialog)
{
	m_rs = rs;
	m_applyed = false;
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);
	ui->shiftEdit->setValue(0);
	connect(ui->shiftEdit, SIGNAL(valueChanged(double)), this, SLOT(checkShiftValue(double)));
	connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(handleButtonClick(QAbstractButton*)));

	bool isMaxSet = false;
	GeoDataRiverPathPoint* p = rs->headPoint();
	p = p->nextPoint();
	while (p != nullptr) {
		if (p->IsSelected) {
			if (! isMaxSet || p->crosssection().leftBank(true).position() > m_leftMax) {
				m_leftMax = p->crosssection().leftBank(true).position();
			}
			if (! isMaxSet || p->crosssection().rightBank(true).position() < m_rightMax) {
				m_rightMax = p->crosssection().rightBank(true).position();
			}
			isMaxSet = true;
		}
		p = p->nextPoint();
	}
	ui->label->setText(tr("Transfer Quality s (%1 < s < %2)").arg(m_leftMax).arg(m_rightMax));
}

GeoDataRiverPathPointShiftDialog::~GeoDataRiverPathPointShiftDialog()
{
	delete ui;
}

void GeoDataRiverPathPointShiftDialog::accept()
{
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
	}
	iRICUndoStack::instance().push(new GeoDataRiverSurvey::ShiftRiverPathCenterCommand(false, ui->shiftEdit->value(), m_rs));
	QDialog::accept();
}

void GeoDataRiverPathPointShiftDialog::reject()
{
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
		customGraphicsUpdate();
	}
	QDialog::reject();
}

void GeoDataRiverPathPointShiftDialog::doReset()
{
	ui->shiftEdit->setValue(0);
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
		customGraphicsUpdate();
	}
	m_applyed = false;
}

void GeoDataRiverPathPointShiftDialog::apply()
{
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
	}
	iRICUndoStack::instance().push(new GeoDataRiverSurvey::ShiftRiverPathCenterCommand(true, ui->shiftEdit->value(), m_rs));
	m_applyed = true;
}

void GeoDataRiverPathPointShiftDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	} else if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ResetRole) {
		doReset();
	}
}

void GeoDataRiverPathPointShiftDialog::customGraphicsUpdate()
{
	m_rs->headPoint()->updateAllXSecInterpolators();
	m_rs->headPoint()->updateRiverShapeInterpolators();
	m_rs->updateShapeData();
	m_rs->renderGraphicsView();
	m_rs->updateCrosssectionWindows();
}

void GeoDataRiverPathPointShiftDialog::checkShiftValue(double shiftVal)
{
	bool ok = m_leftMax < shiftVal && shiftVal < m_rightMax;
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
	ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(ok);
}
