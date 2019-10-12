#include "ui_geodatariverpathpointextensionadddialog.h"

#include "geodatarivercrosssection.h"
#include "geodatariverpathpoint.h"
#include "geodatariverpathpointextensionadddialog.h"
#include "geodatariversurvey.h"
#include "geodatariversurveybackgroundgridcreatethread.h"
#include "private/geodatariversurvey_addextensioncommand.h"

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

void GeoDataRiverPathPointExtensionAddDialog::setLineMode(LineMode lm)
{
	m_lineMode = lm;
}

void GeoDataRiverPathPointExtensionAddDialog::accept()
{
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
	}
	QPointF pos;
	if (ui->clickRadioButton->isChecked()) {
		// set by mouse click.
		pos = QPointF(m_position.x(), m_position.y());
	} else {
		// set by text boxes.
		pos = QPointF(ui->positionXEdit->value(), ui->positionYEdit->value());
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
		m_rs->updateCrosssectionWindows();
	}
	QDialog::reject();
}

void GeoDataRiverPathPointExtensionAddDialog::apply()
{
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
	}
	QPointF pos;
	if (ui->clickRadioButton->isChecked()) {
		// set by mouse click.
		pos = QPointF(m_position.x(), m_position.y());
	} else {
		// set by text boxes.
		pos = QPointF(ui->positionXEdit->value(), ui->positionYEdit->value());
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

void GeoDataRiverPathPointExtensionAddDialog::setPoint(const QPointF &position)
{
	if (! ui->clickRadioButton->isChecked()) {return;}
	m_position = position;
	ui->positionXEdit->setValue(position.x());
	ui->positionYEdit->setValue(position.y());
	apply();
}
