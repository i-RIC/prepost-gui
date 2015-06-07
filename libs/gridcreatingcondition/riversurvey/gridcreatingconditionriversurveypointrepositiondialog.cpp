#include "ui_gridcreatingconditionriversurveypointrepositiondialog.h"

#include "gridcreatingconditionriversurvey.h"
#include "gridcreatingconditionriversurveypointrepositiondialog.h"

#include <misc/iricundostack.h>

#include <QUndoCommand>

/*!
 * Command for repositioning selected control points.
 */
class GridCreatingConditionCtrlPointRepositionCommand : public QUndoCommand
{
public:
	GridCreatingConditionCtrlPointRepositionCommand(
		bool apply,
		RawDataRiverPathPoint::CtrlPointsAddMethod method,
		GridCreatingConditionRiverSurvey* cond)
		: QUndoCommand(GridCreatingConditionRiverSurvey::tr("Reposition Control Points")) {
		m_apply = apply;
		m_condition = cond;
		buildPoints(method);
	}
	void undo() {
		m_point->CtrlPoints(m_position) = m_before;
		if (! m_apply) {
			m_condition->updateShapeData();
			m_condition->renderGraphicsView();
			m_condition->informCtrlPointUpdateToCrosssectionWindows();
		}
	}
	void redo() {
		m_point->CtrlPoints(m_position) = m_after;
		m_condition->updateShapeData();
		m_condition->renderGraphicsView();
		m_condition->informCtrlPointUpdateToCrosssectionWindows();
	}

private:
	void buildPoints(RawDataRiverPathPoint::CtrlPointsAddMethod method) {

		std::list<CtrlPointSelectionInfo>& infoList = m_condition->m_selectedCtrlPointInfoList;
		CtrlPointSelectionInfo Info = infoList.front();
		m_point = Info.Point;
		m_position = Info.Position;
		int minIndex = infoList.front().Index;
		int maxIndex = infoList.back().Index;

		m_before = m_point->CtrlPoints(m_position);
		m_point->reposCtrlPoints(m_position, minIndex, maxIndex, method);
		m_after = m_point->CtrlPoints(m_position);
	}
	bool m_apply;
	GridCreatingConditionRiverSurvey* m_condition;
	RawDataRiverPathPoint* m_point;
	RawDataRiverPathPoint::CtrlPointPosition m_position;
	QVector<double> m_before;
	QVector<double> m_after;
};


GridCreatingConditionRiverSurveyPointRepositionDialog::GridCreatingConditionRiverSurveyPointRepositionDialog(
	GridCreatingConditionRiverSurvey* gc,
	QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GridCreatingConditionRiverSurveyPointRepositionDialog)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);
	adjustSize();

	m_applied = false;
	m_condition = gc;
	ui->ratioSpinBox->setDisabled(true);

	connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(handleButtonClick(QAbstractButton*)));
}

GridCreatingConditionRiverSurveyPointRepositionDialog::~GridCreatingConditionRiverSurveyPointRepositionDialog()
{
	delete ui;
}

void GridCreatingConditionRiverSurveyPointRepositionDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

void GridCreatingConditionRiverSurveyPointRepositionDialog::apply()
{
	if (m_applied) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
	}
	RawDataRiverPathPoint::CtrlPointsAddMethod method = buildMethod();
	iRICUndoStack::instance().push(new GridCreatingConditionCtrlPointRepositionCommand(true, method, m_condition));
	m_applied = true;
}

void GridCreatingConditionRiverSurveyPointRepositionDialog::accept()
{
	if (m_applied) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
	}
	RawDataRiverPathPoint::CtrlPointsAddMethod method = buildMethod();
	iRICUndoStack::instance().push(new GridCreatingConditionCtrlPointRepositionCommand(false, method, m_condition));
	QDialog::accept();
}

void GridCreatingConditionRiverSurveyPointRepositionDialog::reject()
{
	if (m_applied) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
		m_condition->updateShapeData();
		m_condition->renderGraphicsView();
	}
	QDialog::reject();
}

RawDataRiverPathPoint::CtrlPointsAddMethod GridCreatingConditionRiverSurveyPointRepositionDialog::buildMethod()
{
	RawDataRiverPathPoint::CtrlPointsAddMethod method;
	method.number = 1;
	if (ui->uniformRadioButton->isChecked()) {
		method.method = RawDataRiverPathPoint::CtrlPointsAddMethod::am_Uniform;
	} else if (ui->equalRatioRadioButton->isChecked()) {
		method.method = RawDataRiverPathPoint::CtrlPointsAddMethod::am_EqRatio_Ratio;
		method.param = ui->ratioSpinBox->value();
	}
	return method;
}

