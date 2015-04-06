#include "gridcreatingconditionriversurveyregiondialog.h"
#include "ui_gridcreatingconditionriversurveyregiondialog.h"
#include "gridcreatingconditionriversurvey.h"

#include <rawdata/riversurvey/rawdatariversurvey.h>

GridCreatingConditionRiverSurveyRegionDialog::GridCreatingConditionRiverSurveyRegionDialog(GridCreatingConditionRiverSurvey* cond, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GridCreatingConditionRiverSurveyRegionDialog)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);
	m_condition = cond;
	connect(ui->startComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleStartUpdate()));
	connect(ui->endComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleEndUpdate()));
}

GridCreatingConditionRiverSurveyRegionDialog::~GridCreatingConditionRiverSurveyRegionDialog()
{
	delete ui;
}

void GridCreatingConditionRiverSurveyRegionDialog::setData(RawDataRiverSurvey* rs)
{
	RawDataRiverPathPoint* p = rs->headPoint()->nextPoint();
	while (p != 0){
		m_points.push_back(p);
		p = p->nextPoint();
	}
	// setup start combobox
	for (int i = 0; i < m_points.count() - 1; ++i){
		ui->startComboBox->addItem(m_points.at(i)->name());
	}
	// setup end combobox
	for (int i = 1; i < m_points.count(); ++i){
		ui->endComboBox->addItem(m_points.at(i)->name());
	}
	// set default values
	ui->startComboBox->setCurrentIndex(0);
	ui->endComboBox->setCurrentIndex(m_points.count() - 2);
}

void GridCreatingConditionRiverSurveyRegionDialog::setStartPoint(RawDataRiverPathPoint* p)
{
	int index = m_points.indexOf(p);
	if (index == - 1){index = 0;}
	ui->startComboBox->setCurrentIndex(index);
}

void GridCreatingConditionRiverSurveyRegionDialog::setEndPoint(RawDataRiverPathPoint* p)
{
	int index = m_points.indexOf(p);
	if (index == - 1){index = m_points.count() - 1;}
	ui->endComboBox->setCurrentIndex(index - 1);
}

RawDataRiverPathPoint* GridCreatingConditionRiverSurveyRegionDialog::startPoint()
{
	int index = ui->startComboBox->currentIndex();
	if (index < 0 || index > m_points.count() - 1){return 0;}
	return m_points.at(index);
}

RawDataRiverPathPoint* GridCreatingConditionRiverSurveyRegionDialog::endPoint()
{
	int index = ui->endComboBox->currentIndex();
	if (index < 0 || index > m_points.count() - 2){return 0;}
	return m_points.at(index + 1);
}

void GridCreatingConditionRiverSurveyRegionDialog::handleStartUpdate()
{
	int index = ui->startComboBox->currentIndex();
	if (ui->endComboBox->currentIndex() < index){
		ui->endComboBox->setCurrentIndex(index);
	}
	m_condition->selectCreateRegion(startPoint(), endPoint());
}

void GridCreatingConditionRiverSurveyRegionDialog::handleEndUpdate()
{
	int index = ui->endComboBox->currentIndex();
	if (ui->startComboBox->currentIndex() > index){
		ui->startComboBox->setCurrentIndex(index);
	}
	m_condition->selectCreateRegion(startPoint(), endPoint());
}

void GridCreatingConditionRiverSurveyRegionDialog::accept()
{
	m_condition->createGrid(startPoint(), endPoint());
	QDialog::accept();
}
