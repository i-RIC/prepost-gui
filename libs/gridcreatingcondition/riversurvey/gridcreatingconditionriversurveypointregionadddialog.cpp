#include "ui_gridcreatingconditionriversurveypointregionadddialog.h"

#include "gridcreatingconditionriversurvey.h"
#include "gridcreatingconditionriversurvey.h"
#include "gridcreatingconditionriversurveypointregionadddialog.h"

#include <geodata/riversurvey/geodatariverpathpoint.h>
#include <geodata/riversurvey/geodatariversurvey.h>
#include <geodata/riversurvey/geodatariversurveyctrlpointbackup.h>
#include <misc/iricundostack.h>
#include <misc/mathsupport.h>

#include <QMessageBox>
#include <QUndoCommand>

/*!
 * Command for region add of control points.
 */
class GridCreatingConditionCtrlPointRegionAddCommand : public QUndoCommand
{
public:
	GridCreatingConditionCtrlPointRegionAddCommand(
		GeoDataRiverPathPoint* start,
		GeoDataRiverPathPoint* end,
		bool specifydistance,
		int points,
		double distance,
		GridCreatingConditionRiverSurvey* cond)
		: QUndoCommand(GridCreatingConditionRiverSurvey::tr("Add Control Points regionally")) {
		m_condition = cond;
		ctrlPointsRegionAdd(start, end, specifydistance, points, distance);
	}

	void undo() {
		m_condition->cancelBackgroundGridUpdate();
		for (GeoDataRiverSurveyCtrlPointBackup* backup : m_before) {
			backup->restore();
		}

		m_condition->updateShapeData();
		m_condition->renderGraphicsView();
		m_condition->informCtrlPointUpdateToCrosssectionWindows();
	}

	void redo() {
		m_condition->cancelBackgroundGridUpdate();
		for (GeoDataRiverSurveyCtrlPointBackup* backup : m_after) {
			backup->restore();
		}

		m_condition->updateShapeData();
		m_condition->renderGraphicsView();
		m_condition->informCtrlPointUpdateToCrosssectionWindows();
	}

private:
	void ctrlPointsRegionAdd(GeoDataRiverPathPoint* start, GeoDataRiverPathPoint* end, bool specifydistance, int points, double distance) {
		GeoDataRiverPathPoint* tmpp;
		GeoDataRiverSurveyCtrlPointBackup* backup;

		// Make backup.
		tmpp = start;
		while (tmpp != end) {
			backup = new GeoDataRiverSurveyCtrlPointBackup();
			backup->backup(tmpp, GeoDataRiverPathPoint::zposCenterLine);
			m_before.push_back(backup);
			tmpp = tmpp->nextPoint();
		}

		// Set ctrl points data
		tmpp = start;
		while (tmpp != end) {
			int tmppoints = 0;
			if (specifydistance) {
				double tmpdist = iRIC::length(tmpp->position() - tmpp->nextPoint()->position());
				tmppoints = static_cast<int>(floor(tmpdist / distance));
			} else {
				tmppoints = points;
			}
			// points
			QVector<double> tmpv;
			tmpv.reserve(tmppoints);
			double delta = 1. / static_cast<double>(tmppoints + 1);
			for (int i = 0; i < tmppoints; ++i) {
				tmpv.push_back(delta * (i + 1));
			}
			tmpp->CenterLineCtrlPoints = tmpv;
			tmpp->LeftBankCtrlPoints = tmpv;
			tmpp->RightBankCtrlPoints = tmpv;
			tmpp = tmpp->nextPoint();
		}

		// Make backup.
		tmpp = start;
		while (tmpp != end) {
			backup = new GeoDataRiverSurveyCtrlPointBackup();
			backup->backup(tmpp, GeoDataRiverPathPoint::zposCenterLine);
			m_after.push_back(backup);
			tmpp = tmpp->nextPoint();
		}
	}

private:
	GridCreatingConditionRiverSurvey* m_condition;
	std::list<GeoDataRiverSurveyCtrlPointBackup*> m_before;
	std::list<GeoDataRiverSurveyCtrlPointBackup*> m_after;
};


GridCreatingConditionRiverSurveyPointRegionAddDialog::GridCreatingConditionRiverSurveyPointRegionAddDialog(GridCreatingConditionRiverSurvey* cond, QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GridCreatingConditionRiverSurveyPointRegionAddDialog)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);
	m_condition = cond;
	connect(ui->startComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleStartUpdate()));
	connect(ui->endComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleEndUpdate()));
}

GridCreatingConditionRiverSurveyPointRegionAddDialog::~GridCreatingConditionRiverSurveyPointRegionAddDialog()
{
	delete ui;
}

void GridCreatingConditionRiverSurveyPointRegionAddDialog::changeEvent(QEvent* e)
{
	QDialog::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void GridCreatingConditionRiverSurveyPointRegionAddDialog::setData(GeoDataRiverSurvey* rs)
{
	GeoDataRiverPathPoint* p = rs->headPoint()->nextPoint();
	double lengthSum = 0;
	int numSections = 0;
	while (p != nullptr) {
		m_points.push_back(p);
		if (p->nextPoint() != nullptr) {
			lengthSum += iRIC::length(p->position() - p->nextPoint()->position());
			++ numSections;
		}
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
	// set default value for distance.

	if (numSections > 0) {
		ui->distanceSpinBox->setValue(lengthSum / (numSections * 4));
	}
}

void GridCreatingConditionRiverSurveyPointRegionAddDialog::setStartPoint(GeoDataRiverPathPoint* p)
{
	int index = m_points.indexOf(p);
	if (index == - 1) {index = 0;}
	ui->startComboBox->setCurrentIndex(index);
}

void GridCreatingConditionRiverSurveyPointRegionAddDialog::setEndPoint(GeoDataRiverPathPoint* p)
{
	int index = m_points.indexOf(p);
	if (index == - 1) {index = m_points.count() - 2;}
	ui->endComboBox->setCurrentIndex(index);
}

GeoDataRiverPathPoint* GridCreatingConditionRiverSurveyPointRegionAddDialog::startPoint()
{
	int index = ui->startComboBox->currentIndex();
	if (index < 0 || index > m_points.count() - 1) {return 0;}
	return m_points.at(index);
}

GeoDataRiverPathPoint* GridCreatingConditionRiverSurveyPointRegionAddDialog::endPoint()
{
	int index = ui->endComboBox->currentIndex();
	if (index < 0 || index > m_points.count() - 2) {return 0;}
	return m_points.at(index + 1);
}

void GridCreatingConditionRiverSurveyPointRegionAddDialog::handleStartUpdate()
{
	int index = ui->startComboBox->currentIndex();
	if (ui->endComboBox->currentIndex() < index) {
		ui->endComboBox->setCurrentIndex(index);
	}
	m_condition->selectCreateRegion(startPoint(), endPoint());
}

void GridCreatingConditionRiverSurveyPointRegionAddDialog::handleEndUpdate()
{
	int index = ui->endComboBox->currentIndex();
	if (ui->startComboBox->currentIndex() > index) {
		ui->startComboBox->setCurrentIndex(index);
	}
	m_condition->selectCreateRegion(startPoint(), endPoint());
}

void GridCreatingConditionRiverSurveyPointRegionAddDialog::accept()
{
	bool doOK = true;

	if (m_condition->checkCtrlPointsRegion(startPoint(), endPoint())) {
		QMessageBox::StandardButton button = QMessageBox::warning(
			this, tr("Confirmation"),
			tr("The control points which already exist in this region are overwritten."),
			(QMessageBox::Ok | QMessageBox::Cancel),
			QMessageBox::Cancel);
		if (button == QMessageBox::Cancel) {
			doOK = false;
		}
	}

	if (doOK) {
		int divNum = ui->divNumSpinBox->value();
		iRICUndoStack::instance().push(
			new GridCreatingConditionCtrlPointRegionAddCommand(startPoint(), endPoint(), ui->distanceRadioButton->isChecked(), divNum - 1, ui->distanceSpinBox->value(), m_condition));
	}

	QDialog::accept();
}
