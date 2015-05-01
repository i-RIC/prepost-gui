#include "ui_rawdatariverpathpointmovedialog.h"

#include "rawdatariverpathpoint.h"
#include "rawdatariverpathpointmovedialog.h"
#include "rawdatariversurvey.h"
#include "rawdatariversurveybackgroundgridcreatethread.h"

#include <misc/iricundostack.h>

#include <QUndoCommand>

RawDataRiverPathPointMoveDialog::RawDataRiverPathPointMoveDialog(RawDataRiverSurvey* rs, QWidget* parent) :
	QDialog(parent),
	ui(new Ui::RawDataRiverPathPointMoveDialog)
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

RawDataRiverPathPointMoveDialog::~RawDataRiverPathPointMoveDialog()
{
	delete ui;
}


void RawDataRiverPathPointMoveDialog::setCurrentCenter(const QVector2D& current)
{
	m_currentCenter = current;
	ui->coordX->setValue(current.x());
	ui->coordY->setValue(current.y());
}

void RawDataRiverPathPointMoveDialog::setSingleSelection(bool single)
{
	ui->centerPointModeRadioButton->setEnabled(single);
	if (! single) {
		ui->coordX->clear();
		ui->coordY->clear();
	}
}

class RawDataRiverPathPointMoveCommand : public QUndoCommand
{
public:
	RawDataRiverPathPointMoveCommand(bool apply, QVector2D offset, RawDataRiverSurvey* rs)
		: QUndoCommand(RawDataRiverSurvey::tr("Move Traversal Lines")) {
		m_apply = apply;
		RawDataRiverPathPoint* p = rs->headPoint();
		p = p->nextPoint();
		while (p != 0) {
			if (p->IsSelected) {
				m_points.append(p);
				m_oldPositions.append(p->position());
				m_newPositions.append(p->position() + offset);
			}
			p = p->nextPoint();
		}
		m_rs = rs;
	}
	void undo() {
		m_rs->m_gridThread->cancel();

		for (int i = 0; i < m_points.count(); ++i) {
			QVector2D oldpos = m_oldPositions.at(i);
			m_points[i]->setPosition(oldpos);
		}
		if (! m_apply) {
			m_rs->headPoint()->updateRiverShapeInterpolators();
			m_rs->updateShapeData();
			m_rs->renderGraphicsView();
			m_rs->updateCrossectionWindows();
		}
	}
	void redo() {
		m_rs->m_gridThread->cancel();

		for (int i = 0; i < m_points.count(); ++i) {
			QVector2D newpos = m_newPositions.at(i);
			m_points[i]->setPosition(newpos);
		}
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
	RawDataRiverSurvey* m_rs;
};

void RawDataRiverPathPointMoveDialog::accept()
{
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
	}
	QVector2D offset(ui->offsetX->value(), ui->offsetY->value());
	iRICUndoStack::instance().push(new RawDataRiverPathPointMoveCommand(false, offset, m_rs));
	QDialog::accept();
}

void RawDataRiverPathPointMoveDialog::reject()
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

void RawDataRiverPathPointMoveDialog::doReset()
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

void RawDataRiverPathPointMoveDialog::apply()
{
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
	}
	QVector2D offset(ui->offsetX->value(), ui->offsetY->value());
	iRICUndoStack::instance().push(new RawDataRiverPathPointMoveCommand(true, offset, m_rs));
	m_applyed = true;
}

void RawDataRiverPathPointMoveDialog::offsetChange()
{
	if (ui->offsetModeRadioButton->isChecked() && ui->centerPointModeRadioButton->isEnabled()) {
		double offsetx = ui->offsetX->value();
		double offsety = ui->offsetY->value();
		ui->coordX->setValue(m_currentCenter.x() + offsetx);
		ui->coordY->setValue(m_currentCenter.y() + offsety);
	}
}

void RawDataRiverPathPointMoveDialog::centerChange()
{
	if (ui->centerPointModeRadioButton->isChecked()) {
		ui->offsetX->setValue(ui->coordX->value() - m_currentCenter.x());
		ui->offsetY->setValue(ui->coordY->value() - m_currentCenter.y());
	}
}

void RawDataRiverPathPointMoveDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	} else if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ResetRole) {
		doReset();
	}
}
