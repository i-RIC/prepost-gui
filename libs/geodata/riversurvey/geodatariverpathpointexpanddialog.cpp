#include "ui_geodatariverpathpointexpanddialog.h"

#include "geodatariverpathpoint.h"
#include "geodatariverpathpointexpanddialog.h"
#include "geodatariversurvey.h"
#include "geodatariversurveybackgroundgridcreatethread.h"
#include "private/geodatariversurvey_expandcrosssectioncommand.h"

#include <misc/iricundostack.h>

#include <QPushButton>
#include <QUndoCommand>

GeoDataRiverPathPointExpandDialog::GeoDataRiverPathPointExpandDialog(GeoDataRiverSurvey* rs, QWidget* parent) :
	QDialog {parent},
	ui {new Ui::GeoDataRiverPathPointExpandDialog},
	m_rs {rs},
	m_applyed {false}
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);
	GeoDataRiverPathPoint* p = rs->headPoint()->nextPoint();
	bool minDistSet = false;
	while (p != 0) {
		if (p->IsSelected) {
			m_points.append(p);
			double dist = p->crosssection().rightBank(true).position() - p->crosssection().leftBank(true).position();
			if (! minDistSet || dist < m_minDistance) {
				m_minDistance = dist;
				minDistSet = true;
			}
		}
		p = p->nextPoint();
	}
	if (m_points.count() == 1) {
		p = *(m_points.begin());
		m_originalDistance = - p->crosssection().leftBank(true).position();
	}
	initializeWidgets();

	connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(handleButtonClick(QAbstractButton*)));
	connect(ui->ratioEdit, SIGNAL(textChanged(QString)), this, SLOT(ratioChanged()));
	connect(ui->ratioRadioButton, SIGNAL(clicked()), this, SLOT(ratioChanged()));
	connect(ui->incrementEdit, SIGNAL(textChanged(QString)), this, SLOT(incrementChanged()));
	connect(ui->incrementRadioButton, SIGNAL(clicked()), this, SLOT(incrementChanged()));
	connect(ui->distanceEdit, SIGNAL(textChanged(QString)), this, SLOT(distanceChanged()));
	connect(ui->distanceRadioButton, SIGNAL(clicked()), this, SLOT(distanceChanged()));
}

GeoDataRiverPathPointExpandDialog::~GeoDataRiverPathPointExpandDialog()
{
	delete ui;
}

void GeoDataRiverPathPointExpandDialog::initializeWidgets()
{
	ui->ratioRadioButton->setChecked(true);
	ui->incrementEdit->setDisabled(true);
	ui->distanceEdit->setDisabled(true);
	ui->ratioEdit->setValue(1);
	ratioChanged();
}

void GeoDataRiverPathPointExpandDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	} else if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ResetRole) {
		doReset();
	}
}

void GeoDataRiverPathPointExpandDialog::accept()
{
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
	}
	QList<double> ratios;
	getRatios(ratios);
	iRICUndoStack::instance().push(new GeoDataRiverSurvey::ExpandCrosssectionCommand(false, m_points, ratios, m_rs));
	QDialog::accept();
}

void GeoDataRiverPathPointExpandDialog::reject()
{
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
		m_rs->headPoint()->updateAllXSecInterpolators();
		m_rs->headPoint()->updateRiverShapeInterpolators();
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
	}
	QDialog::reject();
}

void GeoDataRiverPathPointExpandDialog::distanceChanged()
{
	if (! ui->distanceRadioButton->isChecked()) {return;}
	if (m_points.count() == 1) {
		GeoDataRiverPathPoint* p = m_points.at(0);
		double ratio = getRatio(p);
		ui->ratioEdit->setValue(ratio);
		ui->incrementEdit->setValue((ratio - 1) * (p->crosssection().rightBank(true).position() - p->crosssection().leftBank(true).position()));
	} else {
		ui->incrementEdit->setText("");
		ui->ratioEdit->setText("");
	}
	double distance = ui->distanceEdit->value();
	updateButtonStatus(distance > 0);
}
void GeoDataRiverPathPointExpandDialog::incrementChanged()
{
	if (! ui->incrementRadioButton->isChecked()) {return;}
	if (m_points.count() == 1) {
		GeoDataRiverPathPoint* p = m_points.at(0);
		double ratio = getRatio(p);
		ui->ratioEdit->setValue(ratio);
		ui->distanceEdit->setValue(m_originalDistance * ratio);
	} else {
		ui->ratioEdit->setText("");
		ui->distanceEdit->setText("");
	}
	double increment = ui->incrementEdit->value();
	updateButtonStatus(increment + m_minDistance > 0);
}

void GeoDataRiverPathPointExpandDialog::ratioChanged()
{
	if (! ui->ratioRadioButton->isChecked()) {return;}
	GeoDataRiverPathPoint* p = m_points.at(0);
	double ratio = getRatio(p);
	if (m_points.count() == 1) {
		ui->incrementEdit->setValue((ratio - 1) * (p->crosssection().rightBank(true).position() - p->crosssection().leftBank(true).position()));
		ui->distanceEdit->setValue(m_originalDistance * ratio);
	} else {
		ui->incrementEdit->setText("");
		ui->distanceEdit->setText("");
	}
}

void GeoDataRiverPathPointExpandDialog::apply()
{
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
	}
	QList<double> ratios;
	getRatios(ratios);
	iRICUndoStack::instance().push(new GeoDataRiverSurvey::ExpandCrosssectionCommand(true, m_points, ratios, m_rs));
	m_applyed = true;
}

void GeoDataRiverPathPointExpandDialog::doReset()
{
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
		m_rs->headPoint()->updateRiverShapeInterpolators();
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
	}
	ui->ratioRadioButton->toggle();
	ui->ratioEdit->setValue(1);
}

double GeoDataRiverPathPointExpandDialog::getRatio(GeoDataRiverPathPoint* p)
{
	double ratio = 0;
	if (ui->incrementRadioButton->isChecked()) {
		double increment = ui->incrementEdit->value();
		double width = p->crosssection().rightBank(true).position() - p->crosssection().leftBank(true).position();
		ratio = (width + increment) / width;
	} else if (ui->distanceRadioButton->isChecked()) {
		double dist = ui->distanceEdit->value();
		ratio = - dist / p->crosssection().leftBank(true).position();
	} else {
		// ratio
		ratio = ui->ratioEdit->value();
	}
	updateButtonStatus(ratio > 0);
	return ratio;
}

void GeoDataRiverPathPointExpandDialog::getRatios(QList<double>& ratios)
{
	ratios.clear();
	for (GeoDataRiverPathPoint* point : m_points) {
		double r = getRatio(point);
		ratios.append(r);
	}
}

void GeoDataRiverPathPointExpandDialog::updateButtonStatus(bool enable)
{
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(enable);
	ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(enable);
}
