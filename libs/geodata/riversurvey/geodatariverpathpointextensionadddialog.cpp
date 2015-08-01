#include "ui_geodatariverpathpointextensionadddialog.h"

#include "geodatarivercrosssection.h"
#include "geodatariverpathpoint.h"
#include "geodatariverpathpointextensionadddialog.h"
#include "geodatariversurvey.h"
#include "geodatariversurveybackgroundgridcreatethread.h"

#include <misc/iricundostack.h>

#include <QUndoCommand>

GeoDataRiverPathPointExtensionAddDialog::GeoDataRiverPathPointExtensionAddDialog(GeoDataRiverPathPoint* p, GeoDataRiverSurvey* rs, QWidget* parent)
	: QDialog(parent),
		ui(new Ui::GeoDataRiverPathPointExtensionAddDialog)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);
	m_point = p;
	m_rs = rs;
	m_applyed = false;
	ui->clickRadioButton->setChecked(true);
	ui->positionXEdit->setEnabled(false);
	ui->positionYEdit->setEnabled(false);

	connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(handleButtonClick(QAbstractButton*)));
}

GeoDataRiverPathPointExtensionAddDialog::~GeoDataRiverPathPointExtensionAddDialog()
{
	delete ui;
}

class GeoDataRiverSurvey::AddExtensionCommand : public QUndoCommand
{
public:
	AddExtensionCommand(bool apply, GeoDataRiverPathPointExtensionAddDialog::LineMode lm, const QVector2D& pos, GeoDataRiverPathPoint* p, GeoDataRiverSurvey* rs) :
		QUndoCommand {GeoDataRiverSurvey::tr("Add Extension Line")}
	{
		m_apply = apply;
		m_lineMode = lm;
		m_position = pos;
		m_point = p;
		m_rs = rs;
	}
	void undo() {
		m_rs->m_gridThread->cancel();
		if (m_lineMode == GeoDataRiverPathPointExtensionAddDialog::Left) {
			m_point->removeExtentionPointLeft();
		} else {
			m_point->removeExtentionPointRight();
		}
		if (! m_apply) {
			m_rs->updateActionStatus();
			m_rs->headPoint()->updateAllXSecInterpolators();
			m_rs->headPoint()->updateRiverShapeInterpolators();
			m_rs->updateShapeData();
			m_rs->renderGraphicsView();
			m_rs->updateCrossectionWindows();
		}
	}
	void redo() {
		m_rs->m_gridThread->cancel();
		if (m_lineMode == GeoDataRiverPathPointExtensionAddDialog::Left) {
			m_point->addExtentionPointLeft(m_position);
		} else {
			m_point->addExtentionPointRight(m_position);
		}
		m_rs->updateActionStatus();
		m_rs->headPoint()->updateAllXSecInterpolators();
		m_rs->headPoint()->updateRiverShapeInterpolators();
		m_rs->setModified();
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
		m_rs->updateCrossectionWindows();
	}
private:
	bool m_apply;
	GeoDataRiverPathPoint* m_point;
	GeoDataRiverSurvey* m_rs;
	GeoDataRiverPathPointExtensionAddDialog::LineMode m_lineMode;
	QVector2D m_position;
};

void GeoDataRiverPathPointExtensionAddDialog::accept()
{
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
	}
	QVector2D pos;
	if (ui->clickRadioButton->isChecked()) {
		// set by mouse click.
		pos = m_position;
	} else {
		// set by text boxes.
		pos = QVector2D(ui->positionXEdit->value(), ui->positionYEdit->value());
	}
	iRICUndoStack::instance().push(new GeoDataRiverSurvey::AddExtensionCommand(false, m_lineMode, pos, m_point, m_rs));
	QDialog::accept();
}

void GeoDataRiverPathPointExtensionAddDialog::reject()
{
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
		m_rs->updateActionStatus();
		m_rs->headPoint()->updateAllXSecInterpolators();
		m_rs->headPoint()->updateRiverShapeInterpolators();
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
		m_rs->updateCrossectionWindows();
	}
	QDialog::reject();
}

void GeoDataRiverPathPointExtensionAddDialog::apply()
{
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
	}
	QVector2D pos;
	if (ui->clickRadioButton->isChecked()) {
		// set by mouse click.
		pos = m_position;
	} else {
		// set by text boxes.
		pos = QVector2D(ui->positionXEdit->value(), ui->positionYEdit->value());
	}
	iRICUndoStack::instance().push(new GeoDataRiverSurvey::AddExtensionCommand(true, m_lineMode, pos, m_point, m_rs));
	m_applyed = true;
}

void GeoDataRiverPathPointExtensionAddDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

void GeoDataRiverPathPointExtensionAddDialog::setPoint(const QVector2D& position)
{
	if (! ui->clickRadioButton->isChecked()) {return;}
	m_position = position;
	ui->positionXEdit->setValue(position.x());
	ui->positionYEdit->setValue(position.y());
	apply();
}
