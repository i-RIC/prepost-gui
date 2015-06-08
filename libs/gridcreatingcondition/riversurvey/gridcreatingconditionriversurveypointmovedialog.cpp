#include "ui_gridcreatingconditionriversurveypointmovedialog.h"

#include "gridcreatingconditionriversurvey.h"
#include "gridcreatingconditionriversurveypointmovedialog.h"

#include <misc/iricundostack.h>

#include <QUndoCommand>

GridCreatingConditionRiverSurveyPointMoveDialog::GridCreatingConditionRiverSurveyPointMoveDialog(
	GridCreatingConditionRiverSurvey* gc, double lowerLimit, double upperLimit, QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GridCreatingConditionRiverSurveyPointMoveDialog)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);

	m_condition = gc;
	m_LowerLimit = lowerLimit;
	m_UpperLimit = upperLimit;

	m_applied = false;

	connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(handleButtonClick(QAbstractButton*)));
}

GridCreatingConditionRiverSurveyPointMoveDialog::~GridCreatingConditionRiverSurveyPointMoveDialog()
{
	delete ui;
}

void GridCreatingConditionRiverSurveyPointMoveDialog::apply()
{
	doReset();
	doOffset(true);
}

void GridCreatingConditionRiverSurveyPointMoveDialog::accept()
{
	doReset();
	doOffset();
	QDialog::accept();
}

void GridCreatingConditionRiverSurveyPointMoveDialog::doOffset(bool preview)
{
	this->setSValue();
	if (m_SValue > 0) {
		iRICUndoStack::instance().push(
			new GridCreatingConditionCtrlPointMoveCommand(preview, m_SValue * m_UpperLimit, m_condition));
	} else {
		iRICUndoStack::instance().push(
			new GridCreatingConditionCtrlPointMoveCommand(preview, - m_SValue * m_LowerLimit, m_condition));
	}
	m_applied = true;
}

void GridCreatingConditionRiverSurveyPointMoveDialog::doReset()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
	}
	m_applied = false;
}

void GridCreatingConditionRiverSurveyPointMoveDialog::setSValue()
{
	m_SValue = ui->sSpinBox->value();
}

void GridCreatingConditionRiverSurveyPointMoveDialog::reject()
{
	this->doReset();
	m_condition->updateShapeData();
	m_condition->renderGraphicsView();
	QDialog::reject();
}

void GridCreatingConditionRiverSurveyPointMoveDialog::on_buttonBox_clicked(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	} else if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ResetRole) {
		doReset();
	}
}
