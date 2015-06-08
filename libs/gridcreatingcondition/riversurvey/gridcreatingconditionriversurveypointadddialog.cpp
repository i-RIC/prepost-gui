#include "ui_gridcreatingconditionriversurveypointadddialog.h"

#include "gridcreatingconditionriversurvey.h"
#include "gridcreatingconditionriversurveypointadddialog.h"

#include <misc/iricundostack.h>
#include <rawdata/riversurvey/rawdatariversurvey.h>
#include <rawdata/riversurvey/rawdatariversurveyctrlpointbackup.h>

#include <QMap>
#include <QUndoCommand>
#include <QVector>

GridCreatingConditionRiverSurveyPointAddDialog::GridCreatingConditionRiverSurveyPointAddDialog(GridCreatingConditionRiverSurvey* cond, QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GridCreatingConditionRiverSurveyPointAddDialog)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);
	ui->ratioSpinBox->setDisabled(true);
	m_condition = cond;
	m_applied = false;
	connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(handleButtonClick(QAbstractButton*)));
	adjustSize();
}

GridCreatingConditionRiverSurveyPointAddDialog::~GridCreatingConditionRiverSurveyPointAddDialog()
{
	delete ui;
}

void GridCreatingConditionRiverSurveyPointAddDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

class GridCreatingConditionRiverSurveyPointAddCommand : public QUndoCommand
{
public:
	GridCreatingConditionRiverSurveyPointAddCommand(bool apply, RawDataRiverPathPoint::CtrlPointsAddMethod method, GridCreatingConditionRiverSurvey* cond)
		: QUndoCommand(GridCreatingConditionRiverSurvey::tr("Add Division Points"))

	{
		m_apply = apply;
		m_condition = cond;
		m_selectedZone = cond->m_selectedZone;
		buildPoints(method);
	}
	void undo() {
		m_condition->cancelBackgroundGridUpdate();
		m_before.restore();
		if (! m_apply) {
			m_condition->updateShapeData();
			m_condition->renderGraphicsView();
			m_condition->informCtrlPointUpdateToCrosssectionWindows();
		} else {
			m_condition->m_selectedZone = m_selectedZone;
		}
	}
	void redo() {
		m_condition->cancelBackgroundGridUpdate();
		m_after.restore();
		m_condition->m_selectedZone.point = 0;
		m_condition->updateShapeData();
		m_condition->renderGraphicsView();
		m_condition->informCtrlPointUpdateToCrosssectionWindows();
	}
private:
	void buildPoints(RawDataRiverPathPoint::CtrlPointsAddMethod method) {
		RawDataRiverPathPoint* tmpp;
		switch (m_condition->m_selectedZone.position) {
		case RawDataRiverPathPoint::zposCenterToLeft:
		case RawDataRiverPathPoint::zposCenterToRight:
			// save the backup
			m_before.backup(m_condition->m_riverSurvey->headPoint(), m_condition->m_selectedZone.position);
			tmpp = NULL;
			tmpp = m_condition->m_riverSurvey->headPoint()->nextPoint();
			while (tmpp) {
				tmpp->addCtrlPoints(m_condition->m_selectedZone.position, m_condition->m_selectedZone.index, method);
				tmpp = tmpp->nextPoint();
			}
			// save the result.
			m_after.backup(m_condition->m_riverSurvey->headPoint(), m_condition->m_selectedZone.position);
			break;
		case RawDataRiverPathPoint::zposCenterLine:
		case RawDataRiverPathPoint::zposLeftBank:
		case RawDataRiverPathPoint::zposRightBank:
			m_before.backup(m_condition->m_selectedZone.point, m_condition->m_selectedZone.position);
			m_condition->m_selectedZone.point->addCtrlPoints(m_condition->m_selectedZone.position, m_condition->m_selectedZone.index, method);
			m_after.backup(m_condition->m_selectedZone.point, m_condition->m_selectedZone.position);
			break;
		}
	}
	RawDataRiverSurveyCtrlPointBackup m_before;
	RawDataRiverSurveyCtrlPointBackup m_after;
	GridCreatingConditionRiverSurvey* m_condition;
	GridCreatingConditionRiverSurvey::ZoneSelectionInfo m_selectedZone;
	bool m_apply;
};

void GridCreatingConditionRiverSurveyPointAddDialog::accept()
{
	if (m_applied) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
	}
	RawDataRiverPathPoint::CtrlPointsAddMethod method = buildMethod();
	iRICUndoStack::instance().push(new GridCreatingConditionRiverSurveyPointAddCommand(false, method, m_condition));

	m_condition->setActionStatus();
	QDialog::accept();
}

void GridCreatingConditionRiverSurveyPointAddDialog::reject()
{
	if (m_applied) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
		m_condition->updateShapeData();
		m_condition->renderGraphicsView();
	}

	m_condition->setActionStatus();
	QDialog::reject();
}

void GridCreatingConditionRiverSurveyPointAddDialog::apply()
{
	if (m_applied) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
	}
	RawDataRiverPathPoint::CtrlPointsAddMethod method = buildMethod();
	iRICUndoStack::instance().push(new GridCreatingConditionRiverSurveyPointAddCommand(true, method, m_condition));
	m_applied = true;
}

RawDataRiverPathPoint::CtrlPointsAddMethod GridCreatingConditionRiverSurveyPointAddDialog::buildMethod()
{
	RawDataRiverPathPoint::CtrlPointsAddMethod method;
	method.number = ui->divNumSpinBox->value() - 1;
	if (ui->uniformRadioButton->isChecked()) {
		method.method = RawDataRiverPathPoint::CtrlPointsAddMethod::am_Uniform;
	} else if (ui->equalRatioRadioButton->isChecked()) {
		method.method = RawDataRiverPathPoint::CtrlPointsAddMethod::am_EqRatio_Ratio;
		method.param = ui->ratioSpinBox->value();
	}
	return method;
}
