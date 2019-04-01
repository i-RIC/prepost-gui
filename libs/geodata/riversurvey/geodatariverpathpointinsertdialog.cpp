#include "ui_geodatariverpathpointinsertdialog.h"

#include "geodatariverpathpoint.h"
#include "geodatariverpathpointinsertdialog.h"
#include "geodatariversurvey.h"
#include "geodatariversurveybackgroundgridcreatethread.h"
#include "private/geodatariversurvey_insertriverpathpointcommand.h"

#include <misc/interpolator.h>
#include <misc/iricundostack.h>
#include <misc/mathsupport.h>

#include <QRegExp>
#include <QRegExpValidator>
#include <QUndoCommand>

#include <sstream>

GeoDataRiverPathPointInsertDialog::GeoDataRiverPathPointInsertDialog(GeoDataRiverPathPoint* target, bool insert, GeoDataRiverSurvey* rs, QWidget* parent) :
	QDialog {parent},
	ui {new Ui::GeoDataRiverPathPointInsertDialog},
	m_insert {insert},
	m_applyed {false}
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);

	QRegExp rx(GeoDataRiverPathPoint::NAME_REGEXP);
	auto validator = new QRegExpValidator(rx, this);
	ui->nameEdit->setValidator(validator);

	if (insert) {
		m_insertTarget = target->previousPoint();
	} else {
		m_insertTarget = target;
	}
	m_newPoint = new GeoDataRiverPathPoint();
	m_rs = rs;
	if ((insert && target->previousPoint()->firstPoint()) || (! insert && target->nextPoint() == nullptr)) {
		ui->coordRatioRadioButton->setDisabled(true);
		ui->crossCreateRadioButton->setDisabled(true);
		m_lineCenter = nullptr;
	} else {
		m_lineCenter = m_insertTarget->riverCenter()->copy();
	}
	initializeItems();
	setDefaultName();
	setDefaultPosition();
	setupComboBox();
	connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(handleButtonClick(QAbstractButton*)));
	connect(ui->coordRatioRadioButton, SIGNAL(toggled(bool)), this, SLOT(handleRatioToggle(bool)));
}

GeoDataRiverPathPointInsertDialog::~GeoDataRiverPathPointInsertDialog()
{
	delete ui;
}

void GeoDataRiverPathPointInsertDialog::initializeItems()
{
	ui->coordClickRadioButton->setChecked(true);
	ui->crossThreeRadioButton->setChecked(true);

	ui->coordXEdit->setDisabled(true);
	ui->coordYEdit->setDisabled(true);
	ui->coordRatioSpinBox->setDisabled(true);

	ui->crossCopyComboBox->setDisabled(true);
	ui->crossNumberBox->setDisabled(true);
	ui->crossCreateRadioButton->setDisabled(true);
}

void GeoDataRiverPathPointInsertDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

void GeoDataRiverPathPointInsertDialog::accept()
{
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
	}
	updatePoint();
	iRICUndoStack::instance().push(new GeoDataRiverSurvey::InsertRiverPathPointCommand(false, m_insertTarget, m_newPoint, m_rs));
	QDialog::accept();
}

void GeoDataRiverPathPointInsertDialog::reject()
{
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
	}
	delete m_newPoint;
	QDialog::reject();
}

void GeoDataRiverPathPointInsertDialog::apply()
{
	if (m_applyed) {
		// undo the apply action.
		iRICUndoStack::instance().undo();
	}
	updatePoint();
	iRICUndoStack::instance().push(new GeoDataRiverSurvey::InsertRiverPathPointCommand(true, m_insertTarget, m_newPoint, m_rs));
	m_applyed = true;
}

void GeoDataRiverPathPointInsertDialog::setPoint(const QPointF &position)
{
	if (! ui->coordClickRadioButton->isChecked()) {return;}
	m_newPoint->setPosition(position);
	ui->coordXEdit->setValue(position.x());
	ui->coordYEdit->setValue(position.y());
	apply();
}

void GeoDataRiverPathPointInsertDialog::updatePoint()
{
	m_newPoint->InhibitInterpolatorUpdate = true;
	// direction.
	QPointF dir;
	double ratio = 0;
	// name
	m_newPoint->setName(ui->nameEdit->text());
	// center point coordinates.
	if (ui->coordClickRadioButton->isChecked()) {
		if (m_insert) {
			dir = m_insertTarget->nextPoint()->position() - m_newPoint->position();
		} else {
			dir = m_newPoint->position() - m_insertTarget->position();
		}
		dir = iRIC::normalize(iRIC::rotateVector270(dir));
		m_newPoint->setCrosssectionDirection(dir);
	} else if (ui->coordValueRadioButton->isChecked()) {
		m_newPoint->setPosition(QPointF(ui->coordXEdit->value(), ui->coordYEdit->value()));
		if (m_insert) {
			dir = m_insertTarget->nextPoint()->position() - m_newPoint->position();
		} else {
			dir = m_newPoint->position() - m_insertTarget->position();
		}
		dir = iRIC::normalize(iRIC::rotateVector270(dir));
		m_newPoint->setCrosssectionDirection(dir);
	} else if (ui->coordRatioRadioButton->isChecked()) {
		ratio = ui->coordRatioSpinBox->value();
		QPointF pos;
		if (m_insert) {
			pos = m_lineCenter->interpolate(1 - ratio);
			// modify direction.
			dir = iRIC::normalize(m_lineCenter->interpolate(1 - ratio) -
						 m_lineCenter->interpolate(1 - ratio - 0.01));
		} else {
			pos = m_lineCenter->interpolate(ratio);
			// modify direction.
			dir = iRIC::normalize(m_lineCenter->interpolate(ratio + 0.01) -
						 m_lineCenter->interpolate(ratio));
		}
		m_newPoint->setPosition(pos);
		ui->coordXEdit->setValue(pos.x());
		ui->coordYEdit->setValue(pos.y());

		iRIC::rotateVector270(dir);
		m_newPoint->setCrosssectionDirection(dir);
	}
	// crosssection.
	if (ui->crossThreeRadioButton->isChecked()) {
		double tmpdbl1, tmpdbl2;
		if (m_insert && m_insertTarget->firstPoint()) {
			tmpdbl1 = m_insertTarget->nextPoint()->crosssection().leftBank(true).position();
			tmpdbl2 = m_insertTarget->nextPoint()->crosssection().rightBank(true).position();
		} else if ((! m_insert) && m_insertTarget->nextPoint() == nullptr) {
			tmpdbl1 = m_insertTarget->crosssection().leftBank(true).position();
			tmpdbl2 = m_insertTarget->crosssection().rightBank(true).position();
		} else {
			tmpdbl1 = 0.5 * (
									m_insertTarget->crosssection().leftBank(true).position() +
									m_insertTarget->nextPoint()->crosssection().leftBank(true).position()
								);
			tmpdbl2 = 0.5 * (
									m_insertTarget->crosssection().rightBank(true).position() +
									m_insertTarget->nextPoint()->crosssection().rightBank(true).position()
								);
		}
		m_newPoint->crosssection().AltitudeInfo().clear();
		m_newPoint->crosssection().addPoint(tmpdbl1, 0);
		m_newPoint->crosssection().addPoint(0, 0);
		m_newPoint->crosssection().addPoint(tmpdbl2, 0);
		int tmpint;
		GeoDataRiverPathPoint* tmpp = m_insertTarget;
		if (tmpp->firstPoint()) {tmpp = tmpp->nextPoint();}
		tmpint = tmpp->CenterToLeftCtrlPoints.size();
		m_newPoint->CenterToLeftCtrlPoints.clear();
		m_newPoint->CenterToLeftCtrlPoints.reserve(tmpint);
		for (int i = 1; i <= tmpint; ++i) {
			m_newPoint->CenterToLeftCtrlPoints.push_back(i / (double)(tmpint + 1));
		}
		tmpint = tmpp->CenterToRightCtrlPoints.size();
		m_newPoint->CenterToRightCtrlPoints.clear();
		m_newPoint->CenterToRightCtrlPoints.reserve(tmpint);
		for (int i = 1; i <= tmpint; ++i) {
			m_newPoint->CenterToRightCtrlPoints.push_back(i / (double)(tmpint + 1));
		}
	} else if (ui->crossCopyRadioButton->isChecked()) {
		// copy crosssection from the specified river path point.
		QString rppName = ui->crossCopyComboBox->currentText();
		GeoDataRiverPathPoint* p = m_rs->m_headPoint->nextPoint();
		while (p != nullptr) {
			if (p->name() == rppName) {
				break;
			}
			p = p->nextPoint();
		}
		// copy altitude info.
		m_newPoint->crosssection().AltitudeInfo() = p->crosssection().AltitudeInfo();
		m_newPoint->CenterToLeftCtrlPoints = p->CenterToLeftCtrlPoints;
		m_newPoint->CenterToRightCtrlPoints = p->CenterToRightCtrlPoints;
	} else if (ui->crossCreateRadioButton->isChecked()) {
		// delete the 0 points added automatically.
		m_newPoint->crosssection().AltitudeInfo().clear();
		int numPoints = ui->crossNumberBox->value() / 2;
		// create crosssection.
		LinearInterpolator1D1 interpolator;
		GeoDataRiverPathPoint* fromP, *toP;
		if (m_insert) {
			fromP = m_insertTarget->nextPoint();
			toP = m_insertTarget;
		} else {
			fromP = m_insertTarget;
			toP = m_insertTarget->nextPoint();
		}
		// center
		double tmpdbl1, tmpdbl2;
		tmpdbl1 = fromP->lXSec()->interpolate(0).height();
		tmpdbl2 = toP->lXSec()->interpolate(0).height();
		interpolator.setValues(tmpdbl1, tmpdbl2);
		m_newPoint->crosssection().addPoint(0, interpolator.interpolate(ratio));

		// left bank.
		QPointF leftbank;
		if (m_insert) {
			leftbank = m_insertTarget->leftBank()->interpolate(1. - ratio);
		} else {
			leftbank = m_insertTarget->leftBank()->interpolate(ratio);
		}
		double leftbankdist = iRIC::length(leftbank - m_newPoint->position());
		for (int i = 1; i <= numPoints; ++i) {
			double tmpt = i / static_cast<double>(numPoints);
			tmpdbl1 = fromP->lXSec()->interpolate(tmpt).height();
			tmpdbl2 = toP->lXSec()->interpolate(tmpt).height();
			interpolator.setValues(tmpdbl1, tmpdbl2);
			m_newPoint->crosssection().addPoint(- leftbankdist * tmpt, interpolator.interpolate(ratio));
		}
		// right bank.
		QPointF rightbank;
		if (m_insert) {
			rightbank = m_insertTarget->rightBank()->interpolate(1. - ratio);
		} else {
			rightbank = m_insertTarget->rightBank()->interpolate(ratio);
		}
		double rightbankdist = iRIC::length(rightbank - m_newPoint->position());
		for (int i = 1; i <= numPoints; ++i) {
			double tmpt = i / static_cast<double>(numPoints);
			tmpdbl1 = fromP->rXSec()->interpolate(tmpt).height();
			tmpdbl2 = toP->rXSec()->interpolate(tmpt).height();
			interpolator.setValues(tmpdbl1, tmpdbl2);
			m_newPoint->crosssection().addPoint(rightbankdist * tmpt, interpolator.interpolate(ratio));
		}
		// create center to left division points.
		unsigned int tmpint;
		if (m_insert) {
			tmpint = static_cast<unsigned int>(m_insertTarget->nextPoint()->CenterToLeftCtrlPoints.size());
		} else {
			tmpint = static_cast<unsigned int>(m_insertTarget->CenterToLeftCtrlPoints.size());
		}
		m_newPoint->CenterToLeftCtrlPoints.clear();
		m_newPoint->CenterToLeftCtrlPoints.reserve(tmpint);
		for (unsigned int i = 0; i < tmpint; ++i) {
			tmpdbl1 = fromP->CenterToLeftCtrlPoints[i];
			tmpdbl2 = toP->CenterToLeftCtrlPoints[i];
			interpolator.setValues(tmpdbl1, tmpdbl2);
			m_newPoint->CenterToLeftCtrlPoints.push_back(interpolator.interpolate(ratio));
		}
		// create center to right division points.
		if (m_insert) {
			tmpint = static_cast<unsigned int>(m_insertTarget->nextPoint()->CenterToRightCtrlPoints.size());
		} else {
			tmpint = static_cast<unsigned int>(m_insertTarget->CenterToRightCtrlPoints.size());
		}
		m_newPoint->CenterToRightCtrlPoints.clear();
		m_newPoint->CenterToRightCtrlPoints.reserve(tmpint);
		for (unsigned int i = 0; i < tmpint; ++i) {
			tmpdbl1 = fromP->CenterToRightCtrlPoints[i];
			tmpdbl2 = toP->CenterToRightCtrlPoints[i];
			interpolator.setValues(tmpdbl1, tmpdbl2);
			m_newPoint->CenterToRightCtrlPoints.push_back(interpolator.interpolate(ratio));
		}
	}
	m_newPoint->InhibitInterpolatorUpdate = false;
	m_newPoint->updateXSecInterpolators();
}

void GeoDataRiverPathPointInsertDialog::setupComboBox()
{
	int index = 0;
	QComboBox* combo = ui->crossCopyComboBox;
	combo->clear();
	GeoDataRiverPathPoint* p = m_rs->m_headPoint->nextPoint();
	int i = 0;
	while (p != nullptr) {
		combo->addItem(p->name());
		if (m_insert && p == m_insertTarget->nextPoint()) {
			index = i;
		}
		if ((! m_insert) && p == m_insertTarget) {
			index = i;
		}
		p = p->nextPoint();
		++i;
	}
	if (index < combo->count()) {
		combo->setCurrentIndex(index);
	}
}

void GeoDataRiverPathPointInsertDialog::setDefaultName()
{
	ui->nameEdit->setText("");
}

void GeoDataRiverPathPointInsertDialog::setDefaultPosition()
{
	QPointF defaultPosition;
	if (m_insert) {
		if (m_insertTarget->firstPoint()) {
			QPointF diff = m_insertTarget->position() - m_insertTarget->nextPoint()->position();
			defaultPosition = m_insertTarget->position() + diff;
		} else {
			defaultPosition = m_insertTarget->position() * 0.5 + m_insertTarget->nextPoint()->position() * 0.5;
		}
	} else {
		if (m_insertTarget->nextPoint() == nullptr) {
			QPointF diff = m_insertTarget->position() - m_insertTarget->previousPoint()->position();
			defaultPosition = m_insertTarget->position() + diff;
		} else {
			defaultPosition = m_insertTarget->position() * 0.5 + m_insertTarget->nextPoint()->position() * 0.5;
		}
	}
	ui->coordXEdit->setValue(defaultPosition.x());
	ui->coordYEdit->setValue(defaultPosition.y());
}

void GeoDataRiverPathPointInsertDialog::handleRatioToggle(bool toggled)
{
	if (! toggled) {
		if (ui->crossCreateRadioButton->isChecked()) {
			ui->crossThreeRadioButton->setChecked(true);
		}
	}
}
