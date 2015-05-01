#include "rawdatariverpathpoint.h"
#include "rawdatariverpathpointrotatedialog.h"
#include "rawdatariversurvey.h"
#include "rawdatariversurveybackgroundgridcreatethread.h"
#include "ui_rawdatariverpathpointrotatedialog.h"

#include <misc/iricundostack.h>
#include <misc/mathsupport.h>

#include <QUndoCommand>

RawDataRiverPathPointRotateDialog::RawDataRiverPathPointRotateDialog(RawDataRiverSurvey* rs, QWidget* parent) :
	QDialog(parent),
	ui(new Ui::RawDataRiverPathPointRotateDialog)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);
	m_rs = rs;
	m_applyed = false;
	ui->incrementRadioButton->setChecked(true);
	ui->incrementEdit->setValue(0);
	ui->relativeEdit->setDisabled(true);
	connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(handleButtonClick(QAbstractButton*)));

	connect(ui->incrementEdit, SIGNAL(valueChanged(double)), this, SLOT(incrementChange()));
	connect(ui->relativeEdit, SIGNAL(valueChanged(double)), this, SLOT(relativeChange()));
}

RawDataRiverPathPointRotateDialog::~RawDataRiverPathPointRotateDialog()
{
	delete ui;
}

void RawDataRiverPathPointRotateDialog::setCurrentRelativeAngle(double current)
{
	m_currentRelativeAngle = current;
	ui->relativeEdit->setValue(current);
}

class RawDataRiverPathPointRotateCommand : public QUndoCommand
{
public:
	RawDataRiverPathPointRotateCommand(bool apply, double angle, RawDataRiverSurvey* rs)
		: QUndoCommand(RawDataRiverSurvey::tr("Rotate Traversal Line")) {
		m_apply = apply;
		RawDataRiverPathPoint* p = rs->headPoint();
		p = p->nextPoint();
		while (p != nullptr) {
			if (p->IsSelected) {
				m_point = p;
				break;
			}
			p = p->nextPoint();
		}
		m_oldDirection = m_point->crosssectionDirection();
		m_newDirection = m_oldDirection;
		iRIC::rotateVector(m_newDirection, angle);
		m_rs = rs;
	}
	void undo() {
		m_rs->m_gridThread->cancel();

		m_point->setCrosssectionDirection(m_oldDirection);
		if (! m_apply) {
			m_rs->updateShapeData();
			m_rs->renderGraphicsView();
		}
	}
	void redo() {
		m_rs->m_gridThread->cancel();

		m_point->setCrosssectionDirection(m_newDirection);

		m_rs->setModified();
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
	}
private:
	bool m_apply;
	RawDataRiverPathPoint* m_point;
	RawDataRiverSurvey* m_rs;
	QVector2D m_oldDirection;
	QVector2D m_newDirection;
};

void RawDataRiverPathPointRotateDialog::accept()
{
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
	}
	iRICUndoStack::instance().push(new RawDataRiverPathPointRotateCommand(false, ui->incrementEdit->value(), m_rs));
	QDialog::accept();
}

void RawDataRiverPathPointRotateDialog::reject()
{
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
	}
	QDialog::reject();
}

void RawDataRiverPathPointRotateDialog::doReset()
{
	ui->relativeRadioButton->setChecked(true);
	ui->relativeEdit->setValue(m_currentRelativeAngle);
	ui->incrementEdit->setValue(0);
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
	}
	m_applyed = false;
}

void RawDataRiverPathPointRotateDialog::apply()
{
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
	}
	iRICUndoStack::instance().push(new RawDataRiverPathPointRotateCommand(true, ui->incrementEdit->value(), m_rs));
	m_applyed = true;
}

void RawDataRiverPathPointRotateDialog::relativeChange()
{
	if (ui->relativeRadioButton->isChecked()) {
		ui->incrementEdit->setValue(ui->relativeEdit->value() - m_currentRelativeAngle);
	}
}

void RawDataRiverPathPointRotateDialog::incrementChange()
{
	if (ui->incrementRadioButton->isChecked()) {
		ui->relativeEdit->setValue(m_currentRelativeAngle + ui->incrementEdit->value());
	}
}

void RawDataRiverPathPointRotateDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	} else if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ResetRole) {
		doReset();
	}
}
