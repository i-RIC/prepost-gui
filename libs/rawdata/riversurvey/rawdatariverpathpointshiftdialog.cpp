#include "rawdatariverpathpoint.h"
#include "rawdatariverpathpointshiftdialog.h"
#include "rawdatariversurvey.h"
#include "rawdatariversurveybackgroundgridcreatethread.h"
#include "ui_rawdatariverpathpointshiftdialog.h"

#include <misc/iricundostack.h>

#include <QPushButton>
#include <QUndoCommand>

RawDataRiverPathPointShiftDialog::RawDataRiverPathPointShiftDialog(RawDataRiverSurvey* rs, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::RawDataRiverPathPointShiftDialog)
{
	m_rs = rs;
	m_applyed = false;
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);
	ui->shiftEdit->setValue(0);
	connect(ui->shiftEdit, SIGNAL(valueChanged(double)), this, SLOT(checkShiftValue(double)));
	connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(handleButtonClick(QAbstractButton*)));

	bool isMaxSet = false;
	RawDataRiverPathPoint* p = rs->headPoint();
	p = p->nextPoint();
	while (p != 0){
		if (p->IsSelected){
			if (! isMaxSet || p->crosssection().leftBank(true).position() > m_leftMax){
				m_leftMax = p->crosssection().leftBank(true).position();
			}
			if (! isMaxSet || p->crosssection().rightBank(true).position() < m_rightMax){
				m_rightMax = p->crosssection().rightBank(true).position();
			}
			isMaxSet = true;
		}
		p = p->nextPoint();
	}
	ui->label->setText(tr("Transfer Quality s (%1 < s < %2)").arg(m_leftMax).arg(m_rightMax));
}

RawDataRiverPathPointShiftDialog::~RawDataRiverPathPointShiftDialog()
{
	delete ui;
}


class RawDataRiverPathPointShiftCenterCommand : public QUndoCommand
{
public:
	RawDataRiverPathPointShiftCenterCommand(bool apply, double offset, RawDataRiverSurvey* rs)
		: QUndoCommand(RawDataRiverSurvey::tr("Shift River Center Points"))
	{
		m_apply = apply;

		RawDataRiverPathPoint* p = rs->headPoint();
		p = p->nextPoint();
		while (p != 0){
			if (p->IsSelected){
				m_points.append(p);
				m_oldPositions.append(p->position());
				m_oldCrosssections.append(p->crosssection().AltitudeInfo());
				p->shiftCenter(offset);
				m_newPositions.append(p->position());
				m_newCrosssections.append(p->crosssection().AltitudeInfo());
			}
			p = p->nextPoint();
		}
		m_rs = rs;
	}
	void undo()
	{
		m_rs->m_gridThread->cancel();
		for (int i = 0; i < m_points.count(); ++i){
			m_points[i]->setPosition(m_oldPositions[i]);
			m_points[i]->crosssection().AltitudeInfo() = m_oldCrosssections[i];
		}
		if (! m_apply){
			m_rs->headPoint()->updateAllXSecInterpolators();
			m_rs->headPoint()->updateRiverShapeInterpolators();
			m_rs->updateShapeData();
			m_rs->renderGraphicsView();
			m_rs->updateCrossectionWindows();
		}
	}
	void redo()
	{
		m_rs->m_gridThread->cancel();
		for (int i = 0; i < m_points.count(); ++i){
			m_points[i]->setPosition(m_newPositions[i]);
			m_points[i]->crosssection().AltitudeInfo() = m_newCrosssections[i];
		}
		m_rs->headPoint()->updateAllXSecInterpolators();
		m_rs->headPoint()->updateRiverShapeInterpolators();
		m_rs->setModified();
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
		m_rs->updateCrossectionWindows();
	}
private:
	bool m_apply;
	QList<RawDataRiverPathPoint*> m_points;
	QList<QVector2D> m_oldPositions;
	QList<QVector2D> m_newPositions;
	QList<RawDataRiverCrosssection::AltitudeList> m_oldCrosssections;
	QList<RawDataRiverCrosssection::AltitudeList> m_newCrosssections;
	RawDataRiverSurvey* m_rs;
};

void RawDataRiverPathPointShiftDialog::accept()
{
	if (m_applyed){
		// undo the apply action.
		iRICUndoStack::instance().undo();
	}
	iRICUndoStack::instance().push(new RawDataRiverPathPointShiftCenterCommand(false, ui->shiftEdit->value(), m_rs));
	QDialog::accept();
}

void RawDataRiverPathPointShiftDialog::reject()
{
	if (m_applyed){
		// undo the apply action.
		iRICUndoStack::instance().undo();
		customGraphicsUpdate();
	}
	QDialog::reject();
}

void RawDataRiverPathPointShiftDialog::doReset()
{
	ui->shiftEdit->setValue(0);
	if (m_applyed){
		// undo the apply action.
		iRICUndoStack::instance().undo();
		customGraphicsUpdate();
	}
	m_applyed = false;
}

void RawDataRiverPathPointShiftDialog::apply()
{
	if (m_applyed){
		// undo the apply action.
		iRICUndoStack::instance().undo();
	}
	iRICUndoStack::instance().push(new RawDataRiverPathPointShiftCenterCommand(true, ui->shiftEdit->value(), m_rs));
	m_applyed = true;
}

void RawDataRiverPathPointShiftDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole){
		apply();
	}else if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ResetRole){
		doReset();
	}
}

void RawDataRiverPathPointShiftDialog::customGraphicsUpdate()
{
	m_rs->headPoint()->updateAllXSecInterpolators();
	m_rs->headPoint()->updateRiverShapeInterpolators();
	m_rs->updateShapeData();
	m_rs->renderGraphicsView();
	m_rs->updateCrossectionWindows();
}

void RawDataRiverPathPointShiftDialog::checkShiftValue(double shiftVal)
{
	bool ok = m_leftMax < shiftVal && shiftVal < m_rightMax;
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
	ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(ok);
}
