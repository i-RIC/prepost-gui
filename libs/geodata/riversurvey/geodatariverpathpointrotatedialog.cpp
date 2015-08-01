#include "ui_geodatariverpathpointrotatedialog.h"

#include "geodatariverpathpoint.h"
#include "geodatariverpathpointrotatedialog.h"
#include "geodatariversurvey.h"
#include "geodatariversurveybackgroundgridcreatethread.h"

#include <misc/iricundostack.h>
#include <misc/mathsupport.h>

#include <QUndoCommand>

GeoDataRiverPathPointRotateDialog::GeoDataRiverPathPointRotateDialog(GeoDataRiverSurvey* rs, QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GeoDataRiverPathPointRotateDialog)
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

GeoDataRiverPathPointRotateDialog::~GeoDataRiverPathPointRotateDialog()
{
	delete ui;
}

void GeoDataRiverPathPointRotateDialog::setCurrentRelativeAngle(double current)
{
	m_currentRelativeAngle = current;
	ui->relativeEdit->setValue(current);
}

class GeoDataRiverSurvey::RotateRiverCrosssectionCommand : public QUndoCommand
{
public:
	RotateRiverCrosssectionCommand(bool apply, double angle, GeoDataRiverSurvey* rs) :
		QUndoCommand {GeoDataRiverSurvey::tr("Rotate Traversal Line")}
	{
		m_apply = apply;
		GeoDataRiverPathPoint* p = rs->headPoint();
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
	GeoDataRiverPathPoint* m_point;
	GeoDataRiverSurvey* m_rs;
	QVector2D m_oldDirection;
	QVector2D m_newDirection;
};

void GeoDataRiverPathPointRotateDialog::accept()
{
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
	}
	iRICUndoStack::instance().push(new GeoDataRiverSurvey::RotateRiverCrosssectionCommand(false, ui->incrementEdit->value(), m_rs));
	QDialog::accept();
}

void GeoDataRiverPathPointRotateDialog::reject()
{
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
	}
	QDialog::reject();
}

void GeoDataRiverPathPointRotateDialog::doReset()
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

void GeoDataRiverPathPointRotateDialog::apply()
{
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
	}
	iRICUndoStack::instance().push(new GeoDataRiverSurvey::RotateRiverCrosssectionCommand(true, ui->incrementEdit->value(), m_rs));
	m_applyed = true;
}

void GeoDataRiverPathPointRotateDialog::relativeChange()
{
	if (ui->relativeRadioButton->isChecked()) {
		ui->incrementEdit->setValue(ui->relativeEdit->value() - m_currentRelativeAngle);
	}
}

void GeoDataRiverPathPointRotateDialog::incrementChange()
{
	if (ui->incrementRadioButton->isChecked()) {
		ui->relativeEdit->setValue(m_currentRelativeAngle + ui->incrementEdit->value());
	}
}

void GeoDataRiverPathPointRotateDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	} else if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ResetRole) {
		doReset();
	}
}
