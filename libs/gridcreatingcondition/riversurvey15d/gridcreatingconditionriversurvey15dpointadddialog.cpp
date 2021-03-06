#include "ui_gridcreatingconditionriversurvey15dpointadddialog.h"

#include "gridcreatingconditioncreatorriversurvey15d.h"
#include "gridcreatingconditionriversurvey15d.h"
#include "gridcreatingconditionriversurvey15dpointadddialog.h"

#include <misc/iricundostack.h>
#include <geodata/riversurvey/geodatariversurveyctrlpointbackup.h>

#include <QUndoCommand>

GridCreatingConditionRiverSurvey15DPointAddDialog::GridCreatingConditionRiverSurvey15DPointAddDialog(GridCreatingConditionRiverSurvey15D* cond, QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GridCreatingConditionRiverSurvey15DPointAddDialog)
{
	ui->setupUi(this);

	setAttribute(Qt::WA_DeleteOnClose);
	ui->ratioSpinBox->setDisabled(true);
	m_condition = cond;
	m_applied = false;
	connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(handleButtonClick(QAbstractButton*)));
	adjustSize();
}

GridCreatingConditionRiverSurvey15DPointAddDialog::~GridCreatingConditionRiverSurvey15DPointAddDialog()
{
	delete ui;
}

void GridCreatingConditionRiverSurvey15DPointAddDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

class GridCreatingConditionRiverSurvey15DPointAddCommand : public QUndoCommand
{
public:
	GridCreatingConditionRiverSurvey15DPointAddCommand(bool apply, GeoDataRiverPathPoint::CtrlPointsAddMethod method, GridCreatingConditionRiverSurvey15D* cond)
		: QUndoCommand(GridCreatingConditionRiverSurvey15D::tr("Add Division Points")) {
		m_apply = apply;
		m_condition = cond;
		m_selectedZone = cond->m_selectedZone;
		buildPoints(method);
	}
	void undo() {
		m_before.restore();
		if (! m_apply) {
			m_condition->updateShapeData();
			m_condition->renderGraphicsView();
		} else {
			m_condition->m_selectedZone = m_selectedZone;
		}
	}
	void redo() {
		m_after.restore();
		m_condition->m_selectedZone.point = nullptr;
		m_condition->updateShapeData();
		m_condition->renderGraphicsView();
	}
private:
	void buildPoints(GeoDataRiverPathPoint::CtrlPointsAddMethod method) {
		switch (m_condition->m_selectedZone.position) {
		case GeoDataRiverPathPoint::zposCenterToLeft:
		case GeoDataRiverPathPoint::zposCenterToRight:
		case GeoDataRiverPathPoint::zposLeftBank:
		case GeoDataRiverPathPoint::zposRightBank:
			// nothing
			break;
		case GeoDataRiverPathPoint::zposCenterLine:
			m_before.backup(m_condition->m_selectedZone.point, m_condition->m_selectedZone.position);
			m_condition->m_selectedZone.point->addCtrlPoints(m_condition->m_selectedZone.position, m_condition->m_selectedZone.index, method);
			m_condition->m_selectedZone.point->LeftBankCtrlPoints.clear();
			m_condition->m_selectedZone.point->RightBankCtrlPoints.clear();
			m_after.backup(m_condition->m_selectedZone.point, m_condition->m_selectedZone.position);
			break;
		}
	}
	GeoDataRiverSurveyCtrlPointBackup m_before;
	GeoDataRiverSurveyCtrlPointBackup m_after;
	GridCreatingConditionRiverSurvey15D* m_condition;
	GridCreatingConditionRiverSurvey15D::ZoneSelectionInfo m_selectedZone;
	bool m_apply;
};

void GridCreatingConditionRiverSurvey15DPointAddDialog::accept()
{
	if (m_applied) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
	}
	GeoDataRiverPathPoint::CtrlPointsAddMethod method = buildMethod();
	iRICUndoStack::instance().push(new GridCreatingConditionRiverSurvey15DPointAddCommand(false, method, m_condition));

	m_condition->setActionStatus();
	QDialog::accept();
}

void GridCreatingConditionRiverSurvey15DPointAddDialog::reject()
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

void GridCreatingConditionRiverSurvey15DPointAddDialog::apply()
{
	if (m_applied) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
	}
	GeoDataRiverPathPoint::CtrlPointsAddMethod method = buildMethod();
	iRICUndoStack::instance().push(new GridCreatingConditionRiverSurvey15DPointAddCommand(true, method, m_condition));
	m_applied = true;
}

GeoDataRiverPathPoint::CtrlPointsAddMethod GridCreatingConditionRiverSurvey15DPointAddDialog::buildMethod()
{
	GeoDataRiverPathPoint::CtrlPointsAddMethod method;
	method.number = ui->divNumSpinBox->value() - 1;
	if (ui->uniformRadioButton->isChecked()) {
		method.method = GeoDataRiverPathPoint::CtrlPointsAddMethod::am_Uniform;
	} else if (ui->equalRatioRadioButton->isChecked()) {
		method.method = GeoDataRiverPathPoint::CtrlPointsAddMethod::am_EqRatio_Ratio;
		method.param = ui->ratioSpinBox->value();
	}
	return method;
}
