#include "ui_geodatarivercrosssectionaltitudemovedialog.h"

#include "geodatarivercrosssection.h"
#include "geodatarivercrosssectionaltitudemovedialog.h"
#include "geodatariverpathpoint.h"
#include "geodatariversurvey.h"
#include "geodatariversurveycrosssectionwindow.h"
#include "private/geodatariversurvey_editcrosssectioncommand.h"

#include <misc/iricundostack.h>

#include <QPushButton>

GeoDataRiverCrosssectionAltitudeMoveDialog::GeoDataRiverCrosssectionAltitudeMoveDialog(GeoDataRiverPathPoint* point, int from, int to, GeoDataRiverSurvey* rs, GeoDataRiverSurveyCrosssectionWindow* w, QWidget* parent) :
	QDialog {parent},
	ui {new Ui::GeoDataRiverCrosssectionAltitudeMoveDialog},
	m_applyed {false},
	m_point {point},
	m_from {from},
	m_to {to},
	m_rs {rs},
	m_window {w}
{
	ui->setupUi(this);
	setLimits();
	ui->horizontalEdit->setValue(0);
	ui->verticalEdit->setValue(0);
	connect(ui->horizontalEdit, SIGNAL(valueChanged(double)), this, SLOT(hOffsetChange()));
	connect(ui->horizontalEdit, SIGNAL(returnPressed()), this, SLOT(hOffsetChange()));
	connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(handleButtonClick(QAbstractButton*)));
}

GeoDataRiverCrosssectionAltitudeMoveDialog::~GeoDataRiverCrosssectionAltitudeMoveDialog()
{
	delete ui;
}

void GeoDataRiverCrosssectionAltitudeMoveDialog::setLimits()
{
	GeoDataRiverCrosssection::AltitudeList& alist = m_point->crosssection().AltitudeInfo();
	QString note;

	if (m_from == 0) {
		m_leftLimitSet = false;
	} else {
		m_leftLimitSet = true;
		m_leftLimit = alist.at(m_from - 1).position() - alist.at(m_from).position();
		note.append(QString(tr("%1 < ")).arg(m_leftLimit));
	}
	note.append(tr("H"));
	if (m_to == alist.size() - 1) {
		m_rightLimitSet = false;
	} else {
		m_rightLimitSet = true;
		m_rightLimit = alist.at(m_to + 1).position() - alist.at(m_to).position();
		note.append(QString(tr(" < %1")).arg(m_rightLimit));
	}
	ui->horizontalNote->setText(note);
}

void GeoDataRiverCrosssectionAltitudeMoveDialog::accept()
{
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
	}
	GeoDataRiverCrosssection::AltitudeList before = m_point->crosssection().AltitudeInfo();
	updateCrosssection();
	GeoDataRiverCrosssection::AltitudeList after = m_point->crosssection().AltitudeInfo();

	iRICUndoStack::instance().push(new GeoDataRiverSurvey::EditCrosssectionCommand(false, tr("Move Elevation Points"), m_point, after, before, m_window, m_rs));
	QDialog::accept();
}

void GeoDataRiverCrosssectionAltitudeMoveDialog::reject()
{
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
		m_rs->updateCrosssectionWindows();
	}
	QDialog::reject();
}

void GeoDataRiverCrosssectionAltitudeMoveDialog::doReset()
{
	ui->horizontalEdit->setValue(0);
	ui->verticalEdit->setValue(0);
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
		m_rs->updateCrosssectionWindows();
	}
	m_applyed = false;
}

void GeoDataRiverCrosssectionAltitudeMoveDialog::apply()
{
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
	}
	GeoDataRiverCrosssection::AltitudeList before = m_point->crosssection().AltitudeInfo();
	updateCrosssection();
	GeoDataRiverCrosssection::AltitudeList after = m_point->crosssection().AltitudeInfo();

	iRICUndoStack::instance().push(new GeoDataRiverSurvey::EditCrosssectionCommand(true, tr("Move Elevation Points"), m_point, after, before, m_window, m_rs));
	m_applyed = true;
}

void GeoDataRiverCrosssectionAltitudeMoveDialog::hOffsetChange()
{
	static bool exec = false;
	// for avoiding recursive execution.
	if (exec) {return;}
	exec = true;

	bool ok = true;
	if (m_leftLimitSet && ui->horizontalEdit->value() <= m_leftLimit) {
		ok = false;
	}
	if (m_rightLimitSet && ui->horizontalEdit->value() >= m_rightLimit) {
		ok = false;
	}
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
	ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(ok);
	exec = false;
}

void GeoDataRiverCrosssectionAltitudeMoveDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	} else if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ResetRole) {
		doReset();
	}
}

void GeoDataRiverCrosssectionAltitudeMoveDialog::updateCrosssection()
{
	GeoDataRiverCrosssection::AltitudeList& alist = m_point->crosssection().AltitudeInfo();
	for (int i = m_from; i <= m_to; ++i) {
		alist[i].setPosition(alist[i].position() + ui->horizontalEdit->value());
		alist[i].setHeight(alist[i].height() + ui->verticalEdit->value());
	}
}
