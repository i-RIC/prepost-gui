#include "ui_gridcreatingconditionriversurvey15dpointmovedialog.h"

#include "gridcreatingconditionriversurvey15d.h"
#include "gridcreatingconditionriversurvey15dpointmovedialog.h"

#include <misc/iricundostack.h>

#include <QUndoCommand>

/*!
 * Command for moving selected control points.
 */
class GridCreatingConditionCtrlPointMoveCommand15D : public QUndoCommand
{
public:
	GridCreatingConditionCtrlPointMoveCommand15D(
		bool apply,
		double offset,
		GridCreatingConditionRiverSurvey15D* cond)
		: QUndoCommand(GridCreatingConditionRiverSurvey15D::tr("Move Control Points")) {
		m_apply = apply;
		m_condition = cond;
		executeGCPOffset(offset);
	}
	void undo() {
		m_point->CtrlPoints(m_position) = m_before;
		m_condition->updateShapeData();
		m_condition->renderGraphicsView();
	}
	void redo() {
		m_point->CtrlPoints(m_position) = m_after;
		m_condition->updateShapeData();
		m_condition->renderGraphicsView();
	}
private:
	void executeGCPOffset(double offset) {
		std::list<CtrlPointSelectionInfo> infoList = m_condition->m_selectedCtrlPointInfoList;
		CtrlPointSelectionInfo Info = infoList.front();
		m_point = Info.Point;
		m_position = Info.Position;

		m_before = m_point->CtrlPoints(m_position);

		int minindex = infoList.front().Index;
		int maxindex = infoList.back().Index;
		for (int i = minindex; i <= maxindex; ++i) {
			Info.Point->CtrlPoints(Info.Position)[i] += offset;
		}

		m_after = m_point->CtrlPoints(m_position);

		m_condition->updateShapeData();
		m_condition->renderGraphicsView();
	}

	bool m_apply;
	GridCreatingConditionRiverSurvey15D* m_condition;
	RawDataRiverPathPoint* m_point;
	RawDataRiverPathPoint::CtrlPointPosition m_position;
	QVector<double> m_before;
	QVector<double> m_after;

};

GridCreatingConditionRiverSurvey15DPointMoveDialog::GridCreatingConditionRiverSurvey15DPointMoveDialog(
	GridCreatingConditionRiverSurvey15D* gc, double lowerLimit, double upperLimit, QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GridCreatingConditionRiverSurvey15DPointMoveDialog)
{
	ui->setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);

	m_condition = gc;
	m_LowerLimit = lowerLimit;
	m_UpperLimit = upperLimit;
	m_applied = false;

	ui->lineEdit_TransferQuantityS->setValidator(
		new QRegExpValidator(QRegExp("(\\-?0\\.[0-9]{0,})|(\\-?0)"), this));
	connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(handleButtonClick(QAbstractButton*)));
}

GridCreatingConditionRiverSurvey15DPointMoveDialog::~GridCreatingConditionRiverSurvey15DPointMoveDialog()
{
	delete ui;
}

void GridCreatingConditionRiverSurvey15DPointMoveDialog::apply()
{
	if (! m_applied) {
		doOffset();
	}
}

void GridCreatingConditionRiverSurvey15DPointMoveDialog::accept()
{
	if (! m_applied) {
		doOffset();
	}
	QDialog::accept();
}

void GridCreatingConditionRiverSurvey15DPointMoveDialog::doOffset()
{
	setSValue();
	if (m_SValue > 0) {
		iRICUndoStack::instance().push(new GridCreatingConditionCtrlPointMoveCommand15D(true, m_SValue * m_UpperLimit, m_condition));
	} else {
		iRICUndoStack::instance().push(new GridCreatingConditionCtrlPointMoveCommand15D(true, - m_SValue * m_LowerLimit, m_condition));
	}
	m_applied = true;
}

void GridCreatingConditionRiverSurvey15DPointMoveDialog::doReset()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
	}
	m_applied = false;
}

void GridCreatingConditionRiverSurvey15DPointMoveDialog::setSValue()
{
	double val;
	bool ok;

	val = QString(ui->lineEdit_TransferQuantityS->text()).toDouble(&ok);
	if (ok) {
		m_SValue = val;
	}
}

void GridCreatingConditionRiverSurvey15DPointMoveDialog::reject()
{
	doReset();
	QDialog::reject();
}

void GridCreatingConditionRiverSurvey15DPointMoveDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	} else if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ResetRole) {
		doReset();
	}
}
