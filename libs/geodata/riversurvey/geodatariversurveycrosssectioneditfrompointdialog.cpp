#include "geodatariverpathpoint.h"
#include "geodatariversurvey.h"
#include "geodatariversurveycrosssectioneditfrompointdialog.h"
#include "geodatariversurveycrosssectionwindow.h"
#include "ui_geodatariversurveycrosssectioneditfrompointdialog.h"
#include "private/geodatariversurvey_editcrosssectioncommand.h"

#include <misc/iricundostack.h>

#include <QMessageBox>

namespace {
	double DELTA = 1.0E-6;
}

GeoDataRiverSurveyCrossSectionEditFromPointDialog::GeoDataRiverSurveyCrossSectionEditFromPointDialog(int startIndex, GeoDataRiverPathPoint* point, GeoDataRiverSurvey* rs, GeoDataRiverSurveyCrosssectionWindow* w, QWidget *parent) :
	QDialog(parent),
	m_startIndex {startIndex},
	m_point {point},
	m_rs {rs},
	m_window {w},
	m_applied {false},
	ui(new Ui::GeoDataRiverSurveyCrossSectionEditFromPointDialog)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);
	connect(ui->continueButton, SIGNAL(clicked()), this, SLOT(continueEdit()));
	connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(handleButtonClick(QAbstractButton*)));
}

GeoDataRiverSurveyCrossSectionEditFromPointDialog::~GeoDataRiverSurveyCrossSectionEditFromPointDialog()
{
	delete ui;
}

void GeoDataRiverSurveyCrossSectionEditFromPointDialog::accept()
{
	reset();
	auto before = m_point->crosssection().AltitudeInfo();
	int newIndex;
	update(&newIndex);
	auto after = m_point->crosssection().AltitudeInfo();
	m_window->setSelectedRow(newIndex);

	iRICUndoStack::instance().push(new GeoDataRiverSurvey::EditCrosssectionCommand(false, tr("Edit Crosssection"), m_point, after, before, m_window, m_rs));
	QDialog::accept();
}

void GeoDataRiverSurveyCrossSectionEditFromPointDialog::reject()
{
	reset();
	m_rs->updateShapeData();
	m_rs->renderGraphicsView();
	m_rs->updateCrossectionWindows();
	QDialog::reject();
}

void GeoDataRiverSurveyCrossSectionEditFromPointDialog::continueEdit()
{
	reset();
	auto before = m_point->crosssection().AltitudeInfo();
	int newIndex;
	update(&newIndex);
	auto after = m_point->crosssection().AltitudeInfo();
	if (ui->leftRadioButton->isChecked()) {
		-- newIndex;
	} else {
		++ newIndex;
	}
	m_startIndex = newIndex;
	m_window->setSelectedRow(newIndex);

	iRICUndoStack::instance().push(new GeoDataRiverSurvey::EditCrosssectionCommand(false, tr("Edit Crosssection"), m_point, after, before, m_window, m_rs));
	m_applied = false;
}

void GeoDataRiverSurveyCrossSectionEditFromPointDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

void GeoDataRiverSurveyCrossSectionEditFromPointDialog::apply()
{
	reset();
	auto before = m_point->crosssection().AltitudeInfo();
	int newIndex;
	update(&newIndex);
	auto after = m_point->crosssection().AltitudeInfo();
	m_window->setSelectedRow(newIndex);

	iRICUndoStack::instance().push(new GeoDataRiverSurvey::EditCrosssectionCommand(true, tr("Edit Crosssection"), m_point, after, before, m_window, m_rs));
	m_applied = true;
}

void GeoDataRiverSurveyCrossSectionEditFromPointDialog::update(int* newIndex)
{
	auto& data = m_point->crosssection().AltitudeInfo();
	auto startPoint = data.at(m_startIndex);
	*newIndex = m_startIndex;

	double distance = ui->distanceSpinBox->value();

	if (ui->leftRadioButton->isChecked()) {
		// left side
		int eraseEnd = m_startIndex;
		int eraseBegin = eraseEnd;
		if (ui->distanceRadioButton->isChecked()) {
			// distance
			double limitPos = startPoint.position() - distance;
			for (int i = 0; i < m_startIndex; ++i) {
				if (data.at(i).position() >= limitPos) {
					eraseBegin = i;
					break;
				}
			}
			*newIndex = m_startIndex - (eraseEnd - eraseBegin) + 1;
			data.push_back(buildAltitudeForDistance());
			data.erase(data.begin() + eraseBegin, data.begin() + eraseEnd);
		} else {
			// cross-point
			if (m_startIndex == 0) {
				showWarningDialogForNotCrossing();
				return;
			}
			CrossCondition nextCond = conditionForLine(data.at(m_startIndex - 1));
			if (nextCond == Hit) {
				// do nothing.
				return;
			}

			int idx = m_startIndex - 2;
			while (idx >= 0) {
				CrossCondition cond = conditionForLine(data.at(idx));
				if (cond == Hit) {
					int eraseBegin = idx + 1;
					int eraseEnd = m_startIndex;
					*newIndex = m_startIndex - (eraseEnd - eraseBegin);
					data.erase(data.begin() + eraseBegin, data.begin() + eraseEnd);
					return;
				}
				if ((cond == Upper && nextCond == Lower) || (cond == Lower && nextCond == Upper)) {
					data.push_back(buildAltitudeForCrossPoint(data.at(idx), data.at(idx + 1)));
					int eraseBegin = idx + 1;
					int eraseEnd = m_startIndex;
					*newIndex = m_startIndex - (eraseEnd - eraseBegin) + 1;
					data.erase(data.begin() + eraseBegin, data.begin() + eraseEnd);
					break;
				}
				-- idx;
			}
			if (idx == -1) {
				showWarningDialogForNotCrossing();
				return;
			}
		}
	} else {
		// right side
		int eraseBegin = m_startIndex + 1;
		int eraseEnd = eraseBegin;
		if (ui->distanceRadioButton->isChecked()) {
			// distance
			double limitPos = startPoint.position() + distance;
			for (int i = m_startIndex + 1; i <= data.size(); ++i) {
				eraseEnd = i;
				if (i == data.size() || data.at(i).position() > limitPos) {
					break;
				}
			}
			data.push_back(buildAltitudeForDistance());
			data.erase(data.begin() + eraseBegin, data.begin() + eraseEnd);
		} else {
			// cross-point
			if (m_startIndex == data.size() - 1) {
				showWarningDialogForNotCrossing();
				return;
			}
			CrossCondition nextCond = conditionForLine(data.at(m_startIndex + 1));
			if (nextCond == Hit) {
				// do nothing.
				return;
			}

			int idx = m_startIndex + 2;
			while (idx <= data.size() - 1) {
				CrossCondition cond = conditionForLine(data.at(idx));
				if (cond == Hit) {
					int eraseBegin = m_startIndex + 1;
					int eraseEnd = idx;
					data.erase(data.begin() + eraseBegin, data.begin() + eraseEnd);
					return;
				}
				if ((cond == Upper && nextCond == Lower) || (cond == Lower && nextCond == Upper)) {
					int eraseBegin = m_startIndex + 1;
					int eraseEnd = idx;
					data.push_back(buildAltitudeForCrossPoint(data.at(idx), data.at(idx - 1)));
					data.erase(data.begin() + eraseBegin, data.begin() + eraseEnd);
					break;
				}
				++ idx;
			}
			if (idx == data.size()) {
				showWarningDialogForNotCrossing();
				return;
			}
		}
	}
	std::sort(data.begin(), data.end());
}

void GeoDataRiverSurveyCrossSectionEditFromPointDialog::reset()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
		m_applied = false;
	}
	m_window->setSelectedRow(m_startIndex);
}

GeoDataRiverCrosssection::Altitude GeoDataRiverSurveyCrossSectionEditFromPointDialog::buildAltitudeForDistance() const
{
	double distance = ui->distanceSpinBox->value();
	double vdist = distance * slope();

	auto& data = m_point->crosssection().AltitudeInfo();
	auto startPoint = data.at(m_startIndex);
	double pos = 0;
	if (ui->leftRadioButton->isChecked()) {
		pos = startPoint.position() - distance;
	} else {
		pos = startPoint.position() + distance;
	}

	GeoDataRiverCrosssection::Altitude alt;
	alt.setPosition(pos);
	alt.setHeight(startPoint.height() + vdist);
	return alt;
}

GeoDataRiverCrosssection::Altitude GeoDataRiverSurveyCrossSectionEditFromPointDialog::buildAltitudeForCrossPoint(const GeoDataRiverCrosssection::Altitude& a1, const GeoDataRiverCrosssection::Altitude& a2) const
{
	double delta1 = heightForLine(a1.position()) - a1.height();
	double delta2 = heightForLine(a2.position()) - a2.height();
	double r = std::fabs(delta2 / (delta2 - delta1));

	GeoDataRiverCrosssection::Altitude alt;
	alt.setPosition(a1.position() * r + a2.position() * (1 - r));
	alt.setHeight(heightForLine(alt.position()));
	return alt;
}

GeoDataRiverSurveyCrossSectionEditFromPointDialog::CrossCondition GeoDataRiverSurveyCrossSectionEditFromPointDialog::conditionForLine(const GeoDataRiverCrosssection::Altitude& alt)
{
	double height = heightForLine(alt.position());
	if (height > alt.height() + DELTA) {
		return Upper;
	} else if (height < alt.height() - DELTA) {
		return Lower;
	} else {
		return Hit;
	}
}

double GeoDataRiverSurveyCrossSectionEditFromPointDialog::heightForLine(double pos) const
{
	auto& data = m_point->crosssection().AltitudeInfo();
	auto startPoint = data.at(m_startIndex);

	double slope = this->slope();
	if (ui->leftRadioButton->isChecked()) {
		slope *= -1;
	}

	double deltaX = pos - startPoint.position();
	return startPoint.height() + deltaX * slope;
}

double GeoDataRiverSurveyCrossSectionEditFromPointDialog::slope() const
{
	double ret = 0;
	if (ui->slopeRadioButton->isChecked()) {
		ret = ui->slopeSpinBox->value() / 100;
	} else {
		ret = 1.0 / ui->fractionSpinBox->value();
	}
	if (ui->upDownComboBox->currentIndex() == 1) {
		ret *= -1;
	}
	return ret;
}

void GeoDataRiverSurveyCrossSectionEditFromPointDialog::showWarningDialogForNotCrossing()
{
	QMessageBox::warning(this, tr("Warning"), tr("There is no cross point. Please check the setting."));
}
