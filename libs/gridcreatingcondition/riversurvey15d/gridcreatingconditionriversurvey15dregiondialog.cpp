#include "ui_gridcreatingconditionriversurvey15dregiondialog.h"

#include "gridcreatingconditionriversurvey15d.h"
#include "gridcreatingconditionriversurvey15dregiondialog.h"

#include <rawdata/riversurvey/rawdatariverpathpoint.h>
#include <rawdata/riversurvey/rawdatariversurvey.h>

#include <QMessageBox>

GridCreatingConditionRiverSurvey15DRegionDialog::GridCreatingConditionRiverSurvey15DRegionDialog(GridCreatingConditionRiverSurvey15D* cond, QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GridCreatingConditionRiverSurvey15DRegionDialog)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);

	m_condition = cond;
	connect(ui->startComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleStartUpdate()));
	connect(ui->endComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleEndUpdate()));
	connect(ui->numberSpinBox, SIGNAL(editingFinished()), this, SLOT(setNumberSpinBox()));
}

GridCreatingConditionRiverSurvey15DRegionDialog::~GridCreatingConditionRiverSurvey15DRegionDialog()
{
	delete ui;
}

void GridCreatingConditionRiverSurvey15DRegionDialog::setData(RawDataRiverSurvey* rs)
{
	RawDataRiverPathPoint* p = rs->headPoint()->nextPoint();
	while (p != nullptr) {
		m_points.push_back(p);
		p = p->nextPoint();
	}
	// setup start combobox
	for (int i = 0; i < m_points.count() - 1; ++i) {
		ui->startComboBox->addItem(m_points.at(i)->name());
	}
	// setup end combobox
	for (int i = 1; i < m_points.count(); ++i) {
		ui->endComboBox->addItem(m_points.at(i)->name());
	}
	// set default values
	ui->startComboBox->setCurrentIndex(0);
	ui->endComboBox->setCurrentIndex(m_points.count() - 2);
}

void GridCreatingConditionRiverSurvey15DRegionDialog::setStartPoint(RawDataRiverPathPoint* p)
{
	int index = m_points.indexOf(p);
	if (index == - 1) { index = 0; }
	ui->startComboBox->setCurrentIndex(index);
}

void GridCreatingConditionRiverSurvey15DRegionDialog::setEndPoint(RawDataRiverPathPoint* p)
{
	int index = m_points.indexOf(p);
	if (index == - 1) { index = m_points.count() - 1; }
	ui->endComboBox->setCurrentIndex(index - 1);
}

RawDataRiverPathPoint* GridCreatingConditionRiverSurvey15DRegionDialog::startPoint()
{
	int index = ui->startComboBox->currentIndex();
	if (index < 0 || index > m_points.count() - 1) { return 0; }
	return m_points.at(index);
}

RawDataRiverPathPoint* GridCreatingConditionRiverSurvey15DRegionDialog::endPoint()
{
	int index = ui->endComboBox->currentIndex();
	if (index < 0 || index > m_points.count() - 2) { return 0; }
	return m_points.at(index + 1);
}

void GridCreatingConditionRiverSurvey15DRegionDialog::handleStartUpdate()
{
	int index = ui->startComboBox->currentIndex();
	if (ui->endComboBox->currentIndex() < index) {
		ui->endComboBox->setCurrentIndex(index);
	}
	m_condition->selectCreateRegion(startPoint(), endPoint());
}

void GridCreatingConditionRiverSurvey15DRegionDialog::handleEndUpdate()
{
	int index = ui->endComboBox->currentIndex();
	if (ui->startComboBox->currentIndex() > index) {
		ui->startComboBox->setCurrentIndex(index);
	}
	m_condition->selectCreateRegion(startPoint(), endPoint());
}

void GridCreatingConditionRiverSurvey15DRegionDialog::setNumberSpinBox()
{
	int val = ui->numberSpinBox->value();
	if (val % 2 == 0) {
		QMessageBox::warning(this, tr("Warning"),
			tr("Number of points in interpolated cross sections should be odd. incrementing by one."),
			QMessageBox::Ok, QMessageBox::Ok);
		ui->numberSpinBox->setValue(val + 1);
	}
}

void GridCreatingConditionRiverSurvey15DRegionDialog::accept()
{
	m_condition->createGrid(startPoint(), endPoint(), ui->numberSpinBox->value());
	QDialog::accept();
}
