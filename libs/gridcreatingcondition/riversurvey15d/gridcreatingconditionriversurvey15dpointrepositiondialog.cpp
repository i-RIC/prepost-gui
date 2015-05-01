#include "ui_gridcreatingconditionriversurvey15dpointrepositiondialog.h"

#include "gridcreatingconditionriversurvey15d.h"
#include "gridcreatingconditionriversurvey15dpointrepositiondialog.h"

#include <misc/iricundostack.h>
#include <rawdata/riversurvey/rawdatariverpathpoint.h>
#include <rawdata/riversurvey/rawdatariversurveyctrlpointbackup.h>

#include <QUndoCommand>

/*!
 * Command for repositioning selected control points.
 */
class GridCreatingConditionCtrlPointRepositionCommand15D : public QUndoCommand
{
public:
	GridCreatingConditionCtrlPointRepositionCommand15D(
		bool apply,
		RawDataRiverPathPoint::CtrlPointsAddMethod method,
		GridCreatingConditionRiverSurvey15D* cond)
		: QUndoCommand(GridCreatingConditionRiverSurvey15D::tr("Reposition Control Points")) {
		m_apply = apply;
		m_condition = cond;
		buildPoints(method);
	}
	void undo() {
		m_point->CtrlPoints(m_position) = m_before;
		if (! m_apply) {
			m_condition->updateShapeData();
			m_condition->renderGraphicsView();
		}
	}
	void redo() {
		m_point->CtrlPoints(m_position) = m_after;
		m_condition->updateShapeData();
		m_condition->renderGraphicsView();
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
	GridCreatingConditionRiverSurvey15D* m_condition;
	RawDataRiverPathPoint* m_point;
	RawDataRiverPathPoint::CtrlPointPosition m_position;
	QVector<double> m_before;
	QVector<double> m_after;
};

GridCreatingConditionRiverSurvey15DPointRepositionDialog::GridCreatingConditionRiverSurvey15DPointRepositionDialog(
	GridCreatingConditionRiverSurvey15D* gc,
	QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GridCreatingConditionRiverSurvey15DPointRepositionDialog)
{
	ui->setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	adjustSize();

	m_applied = false;
	m_condition = gc;
	ui->ratioSpinBox->setDisabled(true);

	connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(handleButtonClick(QAbstractButton*)));
}

GridCreatingConditionRiverSurvey15DPointRepositionDialog::~GridCreatingConditionRiverSurvey15DPointRepositionDialog()
{
	delete ui;
}

void GridCreatingConditionRiverSurvey15DPointRepositionDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

void GridCreatingConditionRiverSurvey15DPointRepositionDialog::apply()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
	}
	RawDataRiverPathPoint::CtrlPointsAddMethod method = buildMethod();
	iRICUndoStack::instance().push(new GridCreatingConditionCtrlPointRepositionCommand15D(true, method, m_condition));
	m_applied = true;
}

void GridCreatingConditionRiverSurvey15DPointRepositionDialog::accept()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
	}
	RawDataRiverPathPoint::CtrlPointsAddMethod method = buildMethod();
	iRICUndoStack::instance().push(new GridCreatingConditionCtrlPointRepositionCommand15D(false, method, m_condition));
	QDialog::accept();
}

void GridCreatingConditionRiverSurvey15DPointRepositionDialog::reject()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
		m_condition->updateShapeData();
		m_condition->renderGraphicsView();
	}
	QDialog::reject();
}

RawDataRiverPathPoint::CtrlPointsAddMethod GridCreatingConditionRiverSurvey15DPointRepositionDialog::buildMethod()
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
