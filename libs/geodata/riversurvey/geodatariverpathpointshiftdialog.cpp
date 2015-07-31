#include "ui_geodatariverpathpointshiftdialog.h"

#include "geodatariverpathpoint.h"
#include "geodatariverpathpointshiftdialog.h"
#include "geodatariversurvey.h"
#include "geodatariversurveybackgroundgridcreatethread.h"

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


class GeoDataRiverSurvey::ShiftRiverPathCenterCommand : public QUndoCommand
{
public:
	ShiftRiverPathCenterCommand(bool apply, double offset, GeoDataRiverSurvey* rs) :
		QUndoCommand {GeoDataRiverSurvey::tr("Shift River Center Points")}
	{
		m_apply = apply;

		GeoDataRiverPathPoint* p = rs->headPoint();
		p = p->nextPoint();
		while (p != nullptr) {
			if (p->IsSelected) {
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
	void undo() {
		m_rs->m_gridThread->cancel();
		for (int i = 0; i < m_points.count(); ++i) {
			m_points[i]->setPosition(m_oldPositions[i]);
			m_points[i]->crosssection().AltitudeInfo() = m_oldCrosssections[i];
		}
		if (! m_apply) {
			m_rs->headPoint()->updateAllXSecInterpolators();
			m_rs->headPoint()->updateRiverShapeInterpolators();
			m_rs->updateShapeData();
			m_rs->renderGraphicsView();
			m_rs->updateCrossectionWindows();
		}
	}
	void redo() {
		m_rs->m_gridThread->cancel();
		for (int i = 0; i < m_points.count(); ++i) {
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
	QList<GeoDataRiverPathPoint*> m_points;
	QList<QVector2D> m_oldPositions;
	QList<QVector2D> m_newPositions;
	QList<GeoDataRiverCrosssection::AltitudeList> m_oldCrosssections;
	QList<GeoDataRiverCrosssection::AltitudeList> m_newCrosssections;
	GeoDataRiverSurvey* m_rs;
};

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
	m_rs->updateCrossectionWindows();
}

void GeoDataRiverPathPointShiftDialog::checkShiftValue(double shiftVal)
{
	bool ok = m_leftMax < shiftVal && shiftVal < m_rightMax;
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
	ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(ok);
}
