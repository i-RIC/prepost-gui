#include "ui_gridcreatingconditionriversurvey15dpointregionadddialog.h"

#include "gridcreatingconditionriversurvey15d.h"
#include "gridcreatingconditionriversurvey15dpointregionadddialog.h"

#include <misc/iricundostack.h>
#include <rawdata/riversurvey/rawdatariverpathpoint.h>
#include <rawdata/riversurvey/rawdatariversurvey.h>
#include <rawdata/riversurvey/rawdatariversurveyctrlpointbackup.h>

#include <QMessageBox>
#include <QUndoCommand>

/*!
 * Command for region add of control points.
 */
class GridCreatingConditionCtrlPointRegionAddCommand15D : public QUndoCommand
{
public:
	GridCreatingConditionCtrlPointRegionAddCommand15D(
		RawDataRiverPathPoint* start,
		RawDataRiverPathPoint* end,
		int points,
		GridCreatingConditionRiverSurvey15D* cond)
		: QUndoCommand(GridCreatingConditionRiverSurvey15D::tr("Add Control Points regionally")) {
		m_condition = cond;
		ctrlPointsRegionAdd(start, end, points);
	}
	void undo() {
		for (RawDataRiverSurveyCtrlPointBackup* backup : m_before) {
			backup->restore();
		}
		m_condition->updateShapeData();
		m_condition->renderGraphicsView();
	}
	void redo() {
		for (RawDataRiverSurveyCtrlPointBackup* backup : m_after) {
			backup->restore();
		}
		m_condition->updateShapeData();
		m_condition->renderGraphicsView();
	}

private:
	void ctrlPointsRegionAdd(RawDataRiverPathPoint* start, RawDataRiverPathPoint* end, int points) {
		RawDataRiverPathPoint* tmpp;
		RawDataRiverSurveyCtrlPointBackup* backup;

		// バックアップを取る。
		tmpp = start;
		while (tmpp != end) {
			backup = new RawDataRiverSurveyCtrlPointBackup();
			backup->backup(tmpp, RawDataRiverPathPoint::zposCenterLine);
			m_before.push_back(backup);
			tmpp = tmpp->nextPoint();
		}

		// 等分割点を作成
		QVector<double> tmpv;
		tmpv.reserve(points);
		double delta = 1. / static_cast<double>(points + 1);
		for (int i = 0; i < points; ++i) {
			tmpv.push_back(delta * (i + 1));
		}

		// 制御点データを設定
		tmpp = start;
		while (tmpp != end) {
			tmpp->CenterLineCtrlPoints = tmpv;
			tmpp = tmpp->nextPoint();
		}

		// バックアップを取る。
		tmpp = start;
		while (tmpp != end) {
			backup = new RawDataRiverSurveyCtrlPointBackup();
			backup->backup(tmpp, RawDataRiverPathPoint::zposCenterLine);
			m_after.push_back(backup);
			tmpp = tmpp->nextPoint();
		}
	}

	GridCreatingConditionRiverSurvey15D* m_condition;
	std::list<RawDataRiverSurveyCtrlPointBackup*> m_before;
	std::list<RawDataRiverSurveyCtrlPointBackup*> m_after;
};

GridCreatingConditionRiverSurvey15DPointRegionAddDialog::GridCreatingConditionRiverSurvey15DPointRegionAddDialog(GridCreatingConditionRiverSurvey15D* cond, QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GridCreatingConditionRiverSurvey15DPointRegionAddDialog)
{
	ui->setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);

	m_condition = cond;
	ui->divNumSpinBox->setValue(2);
	connect(ui->startComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleStartUpdate()));
	connect(ui->endComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleEndUpdate()));
}

GridCreatingConditionRiverSurvey15DPointRegionAddDialog::~GridCreatingConditionRiverSurvey15DPointRegionAddDialog()
{
	delete ui;
}

void GridCreatingConditionRiverSurvey15DPointRegionAddDialog::setData(RawDataRiverSurvey* rs)
{
	RawDataRiverPathPoint* p = rs->headPoint()->nextPoint();
	while (p != nullptr) {
		m_points.push_back(p);
		p = p->nextPoint();
	}
	// setup startComboBox
	for (int i = 0; i < m_points.count() - 1; ++i) {
		ui->startComboBox->addItem(m_points.at(i)->name());
	}
	// setup endComboBox
	for (int i = 1; i < m_points.count(); ++i) {
		ui->endComboBox->addItem(m_points.at(i)->name());
	}
	// set default values
	ui->startComboBox->setCurrentIndex(0);
	ui->endComboBox->setCurrentIndex(m_points.count() - 2);
}

void GridCreatingConditionRiverSurvey15DPointRegionAddDialog::setStartPoint(RawDataRiverPathPoint* p)
{
	int index = m_points.indexOf(p);
	if (index == - 1) { index = 0; }
	ui->startComboBox->setCurrentIndex(index);
}

void GridCreatingConditionRiverSurvey15DPointRegionAddDialog::setEndPoint(RawDataRiverPathPoint* p)
{
	int index = m_points.indexOf(p);
	if (index == - 1) { index = m_points.count() - 2; }
	ui->endComboBox->setCurrentIndex(index);
}

RawDataRiverPathPoint* GridCreatingConditionRiverSurvey15DPointRegionAddDialog::startPoint()
{
	int index = ui->startComboBox->currentIndex();
	if (index < 0 || index > m_points.count() - 1) { return 0; }
	return m_points.at(index);
}

RawDataRiverPathPoint* GridCreatingConditionRiverSurvey15DPointRegionAddDialog::endPoint()
{
	int index = ui->endComboBox->currentIndex();
	if (index < 0 || index > m_points.count() - 2) { return 0; }
	return m_points.at(index + 1);
}

void GridCreatingConditionRiverSurvey15DPointRegionAddDialog::handleStartUpdate()
{
	int index = ui->startComboBox->currentIndex();
	if (ui->endComboBox->currentIndex() < index) {
		ui->endComboBox->setCurrentIndex(index);
	}
	m_condition->selectCreateRegion(startPoint(), endPoint());
}

void GridCreatingConditionRiverSurvey15DPointRegionAddDialog::handleEndUpdate()
{
	int index = ui->endComboBox->currentIndex();
	if (ui->startComboBox->currentIndex() > index) {
		ui->startComboBox->setCurrentIndex(index);
	}
	m_condition->selectCreateRegion(startPoint(), endPoint());
}

void GridCreatingConditionRiverSurvey15DPointRegionAddDialog::accept()
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
		iRICUndoStack::instance().push(new GridCreatingConditionCtrlPointRegionAddCommand15D(startPoint(), endPoint(), divNum - 1, m_condition));
	}
	QDialog::accept();
}
