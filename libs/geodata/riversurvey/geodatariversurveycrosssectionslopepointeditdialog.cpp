#include "geodatariversurveycrosssectionslopepointeditdialog.h"
#include "geodatariversurveycrosssectionwindow.h"
#include "geodatariversurveycrosssectionwindowgraphicsview.h"
#include "ui_geodatariversurveycrosssectionslopepointeditdialog.h"
#include "private/geodatariversurvey_editslopepointcommand.h"

#include <misc/iricundostack.h>
#include <misc/mathsupport.h>

#include <QMessageBox>
#include <algorithm>

GeoDataRiverSurveyCrosssectionSlopePointEditDialog::GeoDataRiverSurveyCrosssectionSlopePointEditDialog(GeoDataRiverSurveyCrosssectionWindow *parent) :
	QDialog(parent),
	m_applied {false},
	ui(new Ui::GeoDataRiverSurveyCrosssectionSlopePointEditDialog)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);
	m_original = parent->target()->crosssection().AltitudeInfo();
	m_originalOdn = parent->target()->odn();

	connect<void (QDoubleSpinBox::*)(double)>(ui->slopeSpinBox, &QDoubleSpinBox::valueChanged, this, &GeoDataRiverSurveyCrosssectionSlopePointEditDialog::handleSlopeEdit);
	connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &GeoDataRiverSurveyCrosssectionSlopePointEditDialog::handleButtonClick);
}

GeoDataRiverSurveyCrosssectionSlopePointEditDialog::~GeoDataRiverSurveyCrosssectionSlopePointEditDialog()
{
	delete ui;
}

void GeoDataRiverSurveyCrosssectionSlopePointEditDialog::setMode(Mode mode)
{
	m_mode = mode;
	if (mode == Mode::LeftAdd)  {
		ui->modeValueLabel->setText(tr("Left Add"));
	} else if (mode == Mode::LeftSub) {
		ui->modeValueLabel->setText(tr("Left Sub"));
	} else if (mode == Mode::RightAdd) {
		ui->modeValueLabel->setText(tr("Right Add"));
	} else if (mode == Mode::RightSub) {
		ui->modeValueLabel->setText(tr("Right Sub"));
	}
}

int GeoDataRiverSurveyCrosssectionSlopePointEditDialog::slope() const
{
	return ui->slopeSpinBox->value();
}

void GeoDataRiverSurveyCrosssectionSlopePointEditDialog::setSlope(int slope)
{
	ui->slopeSpinBox->setValue(slope);
}

void GeoDataRiverSurveyCrosssectionSlopePointEditDialog::calculateLeftAndRightPoints(const GeoDataRiverCrosssection::AltitudeList& alist, double leftShift, Mode mode, const QPointF& point, int slope, QPointF* left, QPointF* right)
{
	double leftX = alist.begin()->position() + leftShift;
	double rightX = alist.rbegin()->position() + leftShift;

	if (mode == Mode::LeftAdd) {
		*left = QPointF(leftX, point.y());
		auto dy = (rightX - point.x()) / slope;
		*right = QPointF(rightX, point.y() - dy);
	} else if (mode == Mode::LeftSub) {
		*right = QPointF(rightX, point.y());
		auto dy = (point.x() - leftX) / slope;
		*left = QPointF(leftX, point.y() + dy);
	} else if (mode == Mode::RightAdd) {
		*right = QPointF(rightX, point.y());
		auto dy = (point.x() - leftX) / slope;
		*left = QPointF(leftX, point.y() - dy);
	} else if (mode == Mode::RightSub) {
		*left = QPointF(leftX, point.y());
		auto dy = (rightX - point.x()) / slope;
		*right = QPointF(rightX, point.y() + dy);
	}
}

void GeoDataRiverSurveyCrosssectionSlopePointEditDialog::accept()
{
	auto com = createCommand(false);
	if (com == nullptr) {
		QMessageBox::warning(this, tr("Warning"), tr("Please specify the setting so that the lines crosses the original cross-section."));
		return;
	}

	if (m_applied) {
		iRICUndoStack::instance().undo();
	}
	iRICUndoStack::instance().push(com);
	QDialog::accept();
}

void GeoDataRiverSurveyCrosssectionSlopePointEditDialog::reject()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
	}

	QDialog::reject();
}

void GeoDataRiverSurveyCrosssectionSlopePointEditDialog::setPoint(const QPointF& point)
{
	if (! ui->clickRadioButton->isChecked()) {return;}

	ui->positionXEdit->setValue(point.x());
	ui->positionYEdit->setValue(point.y());

	apply();
}

void GeoDataRiverSurveyCrosssectionSlopePointEditDialog::handleSlopeEdit(double slope)
{
	QPointF point(ui->positionXEdit->value(), ui->positionYEdit->value());
	crosssectionWindow()->graphicsView()->setSlopePointEditModeSetting(point, slope);
}

void GeoDataRiverSurveyCrosssectionSlopePointEditDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

void GeoDataRiverSurveyCrosssectionSlopePointEditDialog::apply()
{
	auto com = createCommand(true);
	if (com == nullptr) {
		QMessageBox::warning(this, tr("Warning"), tr("Please specify the setting so that the lines crosses the original cross-section."));
		return;
	}

	if (m_applied) {
		iRICUndoStack::instance().undo();
	}
	iRICUndoStack::instance().push(com);

	m_applied = true;
}

GeoDataRiverSurveyCrosssectionWindow* GeoDataRiverSurveyCrosssectionSlopePointEditDialog::crosssectionWindow() const
{
	return dynamic_cast<GeoDataRiverSurveyCrosssectionWindow*> (parentWidget());
}

QUndoCommand* GeoDataRiverSurveyCrosssectionSlopePointEditDialog::createCommand(bool apply) const
{
	QPointF left, right;

	auto target = crosssectionWindow()->target();
	double leftShift = target->crosssection().leftShift();

	QPointF point(ui->positionXEdit->value(), ui->positionYEdit->value());
	calculateLeftAndRightPoints(m_original, leftShift, m_mode, point, ui->slopeSpinBox->value(), &left, &right);

	bool leftFound, rightFound;
	int leftIndex, rightIndex;
	QPointF leftXSec, rightXSec;
	bool usedFallback = false;

	findLeftAndRightCrossSections(m_original, leftShift, point, left, right, &leftFound, &leftIndex, &leftXSec, &rightFound, &rightIndex, &rightXSec, &usedFallback);
	if (! (leftFound && rightFound)) {
		return nullptr;
	}

	GeoDataRiverCrosssection::AltitudeList newAList;
	for (int i = 0; i <= leftIndex; ++i) {
		newAList.push_back(m_original.at(i));
	}
	newAList.push_back(GeoDataRiverCrosssection::Altitude(leftXSec.x() - leftShift, leftXSec.y()));
	if (!usedFallback) {
		newAList.push_back(GeoDataRiverCrosssection::Altitude(point.x() - leftShift, point.y()));
	}
	newAList.push_back(GeoDataRiverCrosssection::Altitude(rightXSec.x() - leftShift, rightXSec.y()));

	for (int i = rightIndex; i < static_cast<int> (m_original.size()); ++i) {
		newAList.push_back(m_original.at(i));
	}

	return new GeoDataRiverSurvey::EditSlopePointCommand(apply, target, newAList, m_original, m_originalOdn, crosssectionWindow());
}

void GeoDataRiverSurveyCrosssectionSlopePointEditDialog::findHorizontalLineIntersections(const GeoDataRiverCrosssection::AltitudeList& alist, double leftShift, double yValue, std::vector<std::pair<QPointF, int>>* intersections)
{
	if (alist.empty()) {
		return;
	}

	double leftX = alist.begin()->position() + leftShift;
	double rightX = alist.rbegin()->position() + leftShift;

	QPointF lineStart(leftX, yValue);
	QPointF lineEnd(rightX, yValue);
	QPointF intersection;
	double r, s;

	for (int i = 0; i < static_cast<int>(alist.size()) - 1; ++i) {
		const auto& a1 = alist.at(i);
		const auto& a2 = alist.at(i + 1);

		QPointF p1(a1.position() + leftShift, a1.height());
		QPointF p2(a2.position() + leftShift, a2.height());

		bool intersect = iRIC::intersectionPoint(lineStart, lineEnd, p1, p2, &intersection, &r, &s);
		if (! intersect || r < 0 || r > 1 || s < 0 || s > 1) {
			continue;
		}

		intersections->push_back(std::make_pair(intersection, i));
	}
}

bool GeoDataRiverSurveyCrosssectionSlopePointEditDialog::selectTwoClosestIntersections(const std::vector<std::pair<QPointF, int>>& intersections, const QPointF& point, int* leftIndex, QPointF* leftXsec, int* rightIndex, QPointF* rightXsec)
{
	if (intersections.size() < 2) {
		return false;
	}

	// Calculate distances from cursor point to each intersection
	std::vector<std::pair<double, size_t>> distances;
	for (size_t i = 0; i < intersections.size(); ++i) {
		double dx = intersections[i].first.x() - point.x();
		double dy = intersections[i].first.y() - point.y();
		double dist = dx * dx + dy * dy; // Use squared distance to avoid sqrt
		distances.push_back(std::make_pair(dist, i));
	}

	// Sort by distance (closest first)
	std::sort(distances.begin(), distances.end());

	// Select the 2 closest intersections
	// Assign the one with smaller x as left, the other as right
	size_t idx1 = distances[0].second;
	size_t idx2 = distances[1].second;

	if (intersections[idx1].first.x() < intersections[idx2].first.x()) {
		*leftIndex = intersections[idx1].second;
		*leftXsec = intersections[idx1].first;
		*rightIndex = intersections[idx2].second + 1;
		*rightXsec = intersections[idx2].first;
	} else {
		*leftIndex = intersections[idx2].second;
		*leftXsec = intersections[idx2].first;
		*rightIndex = intersections[idx1].second + 1;
		*rightXsec = intersections[idx1].first;
	}

	return true;
}

void GeoDataRiverSurveyCrosssectionSlopePointEditDialog::findLeftAndRightCrossSections(const GeoDataRiverCrosssection::AltitudeList& alist, double leftShift, const QPointF& point, const QPointF& left, const QPointF& right, bool* leftFound, int* leftIndex, QPointF* leftXsec, bool* rightFound, int* rightIndex, QPointF* rightXsec, bool* usedFallback)
{
	int index = -1;

	*leftFound = false;
	for (int i = 0; i < alist.size(); ++i) {
		const auto& a = alist.at(i);
		if (a.position() + leftShift >= point.x()) {
			index = i;
			break;
		}
	}
	QPointF intersection;
	double r, s;

	if (index != -1) {
		for (int i = index - 1; i >= 0; --i) {
			const auto& a1 = alist.at(i);
			const auto& a2 = alist.at(i + 1);

			QPointF p1(a1.position() + leftShift, a1.height());
			QPointF p2(a2.position() + leftShift, a2.height());

			bool intersect = iRIC::intersectionPoint(point, left, p1, p2, &intersection, &r, &s);
			if (! intersect || r < 0 || r > 1 || s < 0 || s > 1) {continue;}

			*leftFound = true;
			*leftIndex = i;
			*leftXsec = intersection;
			break;
		}
	}

	index = -1;
	*rightFound = false;
	for (int i = 0; i < alist.size(); ++i) {
		const auto& a = alist.at(i);
		if (a.position() + leftShift > point.x()) {
			index = i - 1;
			break;
		}
	}

	if (index != -1) {
		for (int i = index; i < static_cast<int> (alist.size()) - 1; ++i) {
			const auto& a1 = alist.at(i);
			const auto& a2 = alist.at(i + 1);

			QPointF p1(a1.position() + leftShift, a1.height());
			QPointF p2(a2.position() + leftShift, a2.height());

			bool intersect = iRIC::intersectionPoint(point, right, p1, p2, &intersection, &r, &s);
			if (! intersect || r < 0 || r > 1 || s < 0 || s > 1) {continue;}

			*rightFound = true;
			*rightIndex = i + 1;
			*rightXsec = intersection;
			break;
		}
	}

	// Fallback: If slope line intersections not found, try horizontal line
	if (!(*leftFound && *rightFound)) {
		std::vector<std::pair<QPointF, int>> horizontalIntersections;
		findHorizontalLineIntersections(alist, leftShift, point.y(), &horizontalIntersections);

		if (selectTwoClosestIntersections(horizontalIntersections, point, leftIndex, leftXsec, rightIndex, rightXsec)) {
			*leftFound = true;
			*rightFound = true;
			if (usedFallback != nullptr) {
				*usedFallback = true;
			}
		}
	}
}
