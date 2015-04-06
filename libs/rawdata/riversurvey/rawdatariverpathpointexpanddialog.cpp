#include "rawdatariverpathpoint.h"
#include "rawdatariverpathpointexpanddialog.h"
#include "rawdatariversurvey.h"
#include "rawdatariversurveybackgroundgridcreatethread.h"
#include "ui_rawdatariverpathpointexpanddialog.h"

#include <misc/iricundostack.h>

#include <QPushButton>
#include <QUndoCommand>

RawDataRiverPathPointExpandDialog::RawDataRiverPathPointExpandDialog(RawDataRiverSurvey* rs, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::RawDataRiverPathPointExpandDialog)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);
	m_rs = rs;
	m_applyed = false;
	RawDataRiverPathPoint* p = rs->headPoint()->nextPoint();
	bool minDistSet = false;
	while (p != 0){
		if (p->IsSelected){
			m_points.append(p);
			double dist = p->crosssection().rightBank(true).position() - p->crosssection().leftBank(true).position();
			if (! minDistSet || dist < m_minDistance){
				m_minDistance = dist;
				minDistSet = true;
			}
		}
		p = p->nextPoint();
	}
	if (m_points.count() == 1){
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

RawDataRiverPathPointExpandDialog::~RawDataRiverPathPointExpandDialog()
{
	delete ui;
}

void RawDataRiverPathPointExpandDialog::initializeWidgets()
{
	ui->ratioRadioButton->setChecked(true);
	ui->incrementEdit->setDisabled(true);
	ui->distanceEdit->setDisabled(true);
	ui->ratioEdit->setValue(1);
	ratioChanged();
}

void RawDataRiverPathPointExpandDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole){
		apply();
	}else if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ResetRole){
		doReset();
	}
}

class RawDataRiverPathPointExpandCommand : public QUndoCommand
{
public:
	RawDataRiverPathPointExpandCommand(bool apply, QList<RawDataRiverPathPoint*>& points, QList<double> ratios, RawDataRiverSurvey* rs)
		: QUndoCommand(RawDataRiverSurvey::tr("Expand Traversal Lines"))
	{
		m_apply = apply;
		m_points = points;
		for (int i = 0; i < points.count(); ++i){
			m_oldCrosssections.append(points[i]->crosssection().AltitudeInfo());
			m_points[i]->crosssection().expand(ratios[i]);
			m_newCrosssections.append(points[i]->crosssection().AltitudeInfo());
		}
		m_rs = rs;
	}
	void undo()
	{
		m_rs->m_gridThread->cancel();
		for (int i = 0; i < m_points.count(); ++i){
			m_points[i]->crosssection().AltitudeInfo() = m_oldCrosssections[i];
		}
		if (! m_apply){
			m_rs->headPoint()->updateAllXSecInterpolators();
			m_rs->headPoint()->updateRiverShapeInterpolators();
			m_rs->updateShapeData();
			m_rs->renderGraphicsView();
			m_rs->updateCrossectionWindows();
		}
	}
	void redo()
	{
		m_rs->m_gridThread->cancel();
		for (int i = 0; i < m_points.count(); ++i){
			m_points[i]->crosssection().AltitudeInfo() = m_newCrosssections[i];
		}
		m_rs->headPoint()->updateAllXSecInterpolators();
		m_rs->headPoint()->updateRiverShapeInterpolators();
		m_rs->setModified();
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
		m_rs->updateCrossectionWindows();
	}
private:
	bool m_apply;
	QList<RawDataRiverPathPoint*> m_points;
	QList<RawDataRiverCrosssection::AltitudeList> m_newCrosssections;
	QList<RawDataRiverCrosssection::AltitudeList> m_oldCrosssections;
	RawDataRiverSurvey* m_rs;
};


void RawDataRiverPathPointExpandDialog::accept()
{
	if (m_applyed){
		// undo the apply action.
		iRICUndoStack::instance().undo();
	}
	QList<double> ratios;
	getRatios(ratios);
	iRICUndoStack::instance().push(new RawDataRiverPathPointExpandCommand(false, m_points, ratios, m_rs));
	QDialog::accept();
}

void RawDataRiverPathPointExpandDialog::reject()
{
	if (m_applyed){
		// undo the apply action.
		iRICUndoStack::instance().undo();
		m_rs->headPoint()->updateAllXSecInterpolators();
		m_rs->headPoint()->updateRiverShapeInterpolators();
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
	}
	QDialog::reject();
}

void RawDataRiverPathPointExpandDialog::distanceChanged()
{
	if (! ui->distanceRadioButton->isChecked()){return;}
	if (m_points.count() == 1){
		RawDataRiverPathPoint* p = m_points.at(0);
		double ratio = getRatio(p);
		ui->ratioEdit->setValue(ratio);
		ui->incrementEdit->setValue((ratio - 1) * (p->crosssection().rightBank(true).position() - p->crosssection().leftBank(true).position()));
	}else{
		ui->incrementEdit->setText("");
		ui->ratioEdit->setText("");
	}
	double distance = ui->distanceEdit->value();
	updateButtonStatus(distance > 0);
}
void RawDataRiverPathPointExpandDialog::incrementChanged()
{
	if (! ui->incrementRadioButton->isChecked()){return;}
	if (m_points.count() == 1){
		RawDataRiverPathPoint* p = m_points.at(0);
		double ratio = getRatio(p);
		ui->ratioEdit->setValue(ratio);
		ui->distanceEdit->setValue(m_originalDistance * ratio);
	}else{
		ui->ratioEdit->setText("");
		ui->distanceEdit->setText("");
	}
	double increment = ui->incrementEdit->value();
	updateButtonStatus(increment + m_minDistance > 0);
}

void RawDataRiverPathPointExpandDialog::ratioChanged()
{
	if (! ui->ratioRadioButton->isChecked()){return;}
	RawDataRiverPathPoint* p = m_points.at(0);
	double ratio = getRatio(p);
	if (m_points.count() == 1){
		ui->incrementEdit->setValue((ratio - 1) * (p->crosssection().rightBank(true).position() - p->crosssection().leftBank(true).position()));
		ui->distanceEdit->setValue(m_originalDistance * ratio);
	}else{
		ui->incrementEdit->setText("");
		ui->distanceEdit->setText("");
	}
}

void RawDataRiverPathPointExpandDialog::apply()
{
	if (m_applyed){
		// undo the apply action.
		iRICUndoStack::instance().undo();
	}
	QList<double> ratios;
	getRatios(ratios);
	iRICUndoStack::instance().push(new RawDataRiverPathPointExpandCommand(true, m_points, ratios, m_rs));
	m_applyed = true;
}

void RawDataRiverPathPointExpandDialog::doReset()
{
	if (m_applyed){
		// undo the apply action.
		iRICUndoStack::instance().undo();
		m_rs->headPoint()->updateRiverShapeInterpolators();
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
	}
	ui->ratioRadioButton->toggle();
	ui->ratioEdit->setValue(1);
}

double RawDataRiverPathPointExpandDialog::getRatio(RawDataRiverPathPoint* p){
	double ratio = 0;
	if (ui->incrementRadioButton->isChecked()){
		double increment = ui->incrementEdit->value();
		double width = p->crosssection().rightBank(true).position() - p->crosssection().leftBank(true).position();
		ratio = (width + increment) / width;
	}else if (ui->distanceRadioButton->isChecked()){
		double dist = ui->distanceEdit->value();
		ratio = - dist / p->crosssection().leftBank(true).position();
	}else{
		// ratio
		ratio = ui->ratioEdit->value();
	}
	updateButtonStatus(ratio > 0);
	return ratio;
}

void RawDataRiverPathPointExpandDialog::getRatios(QList<double>& ratios)
{
	ratios.clear();
	QList<RawDataRiverPathPoint*>::iterator it;
	for (it = m_points.begin(); it != m_points.end(); ++it){
		double r = getRatio(*it);
		ratios.append(r);
	}
}

void RawDataRiverPathPointExpandDialog::updateButtonStatus(bool enable){
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(enable);
	ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(enable);
}
