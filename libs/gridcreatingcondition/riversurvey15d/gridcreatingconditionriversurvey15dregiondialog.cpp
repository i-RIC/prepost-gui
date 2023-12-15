#include "ui_gridcreatingconditionriversurvey15dregiondialog.h"

#include "gridcreatingconditionriversurvey15d.h"
#include "gridcreatingconditionriversurvey15dregiondialog.h"

#include <geodata/riversurvey/geodatariverpathpoint.h>
#include <geodata/riversurvey/geodatariversurvey.h>
#include <guicore/pre/base/preprocessorgridcreatingconditiondataiteminterface.h>

#include <QMessageBox>
#include <QPushButton>

GridCreatingConditionRiverSurvey15DRegionDialog::GridCreatingConditionRiverSurvey15DRegionDialog(GridCreatingConditionRiverSurvey15D* cond, QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GridCreatingConditionRiverSurvey15DRegionDialog)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);
	ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("&Create Grid"));

	m_condition = cond;
	connect<void (QComboBox::*)(int)>(ui->startComboBox, &QComboBox::currentIndexChanged, this, &GridCreatingConditionRiverSurvey15DRegionDialog::handleStartUpdate);
	connect<void (QComboBox::*)(int)>(ui->endComboBox, &QComboBox::currentIndexChanged, this, &GridCreatingConditionRiverSurvey15DRegionDialog::handleEndUpdate);
	connect(ui->numberSpinBox, &QSpinBox::editingFinished, this, &GridCreatingConditionRiverSurvey15DRegionDialog::setNumberSpinBox);

	adjustSize();
}

GridCreatingConditionRiverSurvey15DRegionDialog::~GridCreatingConditionRiverSurvey15DRegionDialog()
{
	delete ui;
}

void GridCreatingConditionRiverSurvey15DRegionDialog::setReadOnly(bool readOnly)
{
	ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(readOnly);
}

void GridCreatingConditionRiverSurvey15DRegionDialog::setData(GeoDataRiverSurvey* rs)
{
	GeoDataRiverPathPoint* p = rs->headPoint()->nextPoint();
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

void GridCreatingConditionRiverSurvey15DRegionDialog::setStartPoint(GeoDataRiverPathPoint* p)
{
	int index = m_points.indexOf(p);
	if (index == - 1) { index = 0; }
	ui->startComboBox->setCurrentIndex(index);
}

void GridCreatingConditionRiverSurvey15DRegionDialog::setEndPoint(GeoDataRiverPathPoint* p)
{
	int index = m_points.indexOf(p);
	if (index == - 1) { index = m_points.count() - 1; }
	ui->endComboBox->setCurrentIndex(index - 1);
}

void GridCreatingConditionRiverSurvey15DRegionDialog::setPositionMode(PositionMode mode)
{
	if (mode == PositionMode::LeftBank) {
		ui->positionComboBox->setCurrentIndex(0);
	} else {
		ui->positionComboBox->setCurrentIndex(1);
	}
}

GeoDataRiverPathPoint* GridCreatingConditionRiverSurvey15DRegionDialog::startPoint() const
{
	int index = ui->startComboBox->currentIndex();
	if (index < 0 || index > m_points.count() - 1) { return 0; }
	return m_points.at(index);
}

GeoDataRiverPathPoint* GridCreatingConditionRiverSurvey15DRegionDialog::endPoint() const
{
	int index = ui->endComboBox->currentIndex();
	if (index < 0 || index > m_points.count() - 2) { return 0; }
	return m_points.at(index + 1);
}

GridCreatingConditionRiverSurvey15DRegionDialog::PositionMode GridCreatingConditionRiverSurvey15DRegionDialog::positionMode() const
{
	switch (ui->positionComboBox->currentIndex()) {
	case 0:
		return PositionMode::LeftBank;
		break;
	case 1:
		return PositionMode::CenterPoint;
		break;
	}
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
	bool ok = m_condition->gccDataItem()->confirmOverwriteIfNeeded(this);
	if (! ok) {return;}

	m_condition->createGrid(startPoint(), endPoint(), ui->numberSpinBox->value(), positionMode());
	QDialog::accept();
}
