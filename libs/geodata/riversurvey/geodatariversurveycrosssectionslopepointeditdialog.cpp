#include "geodatariversurveycrosssectionslopepointeditdialog.h"
#include "geodatariversurveycrosssectionwindow.h"
#include "geodatariversurveycrosssectionwindowgraphicsview.h"
#include "ui_geodatariversurveycrosssectionslopepointeditdialog.h"
#include "private/geodatariversurvey_editslopepointcommand.h"

#include <misc/iricundostack.h>
#include <misc/mathsupport.h>

#include <QMessageBox>

GeoDataRiverSurveyCrosssectionSlopePointEditDialog::GeoDataRiverSurveyCrosssectionSlopePointEditDialog(GeoDataRiverSurveyCrosssectionWindow *parent) :
	QDialog(parent),
	m_applied {false},
	ui(new Ui::GeoDataRiverSurveyCrosssectionSlopePointEditDialog)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);
	m_original = parent->target()->crosssection().AltitudeInfo();

	connect<void (QSpinBox::*)(int)>(ui->slopeSpinBox, &QSpinBox::valueChanged, this, &GeoDataRiverSurveyCrosssectionSlopePointEditDialog::handleSlopeEdit);
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

void GeoDataRiverSurveyCrosssectionSlopePointEditDialog::calculateLeftAndRightPoints(const GeoDataRiverCrosssection::AltitudeList& alist, Mode mode, const QPointF& point, int slope, QPointF* left, QPointF* right)
{
	double leftX = alist.begin()->position();
	double rightX = alist.rbegin()->position();

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

void GeoDataRiverSurveyCrosssectionSlopePointEditDialog::handleSlopeEdit(int slope)
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
	QPointF point(ui->positionXEdit->value(), ui->positionYEdit->value());
	calculateLeftAndRightPoints(m_original, m_mode, point, ui->slopeSpinBox->value(), &left, &right);

	bool leftFound, rightFound;
	int leftIndex, rightIndex;
	QPointF leftXSec, rightXSec;

	findLeftAndRightCrossSections(m_original, point, left, right, &leftFound, &leftIndex, &leftXSec, &rightFound, &rightIndex, &rightXSec);
	if (! (leftFound && rightFound)) {
		return nullptr;
	}

	GeoDataRiverCrosssection::AltitudeList newAList;
	for (int i = 0; i <= leftIndex; ++i) {
		newAList.push_back(m_original.at(i));
	}
	newAList.push_back(GeoDataRiverCrosssection::Altitude(leftXSec.x(), leftXSec.y()));
	newAList.push_back(GeoDataRiverCrosssection::Altitude(point.x(), point.y()));
	newAList.push_back(GeoDataRiverCrosssection::Altitude(rightXSec.x(), rightXSec.y()));

	for (int i = rightIndex; i < static_cast<int> (m_original.size()); ++i) {
		newAList.push_back(m_original.at(i));
	}

	return new GeoDataRiverSurvey::EditSlopePointCommand(apply, target, newAList, m_original, crosssectionWindow());
}

void GeoDataRiverSurveyCrosssectionSlopePointEditDialog::findLeftAndRightCrossSections(const GeoDataRiverCrosssection::AltitudeList& alist, const QPointF& point, const QPointF& left, const QPointF& right, bool* leftFound, int* leftIndex, QPointF* leftXsec, bool* rightFound, int* rightIndex, QPointF* rightXsec)
{
	int index = -1;

	*leftFound = false;
	for (int i = 0; i < alist.size(); ++i) {
		const auto& a = alist.at(i);
		if (a.position() >= point.x()) {
			index = i;
			break;
		}
	}
	QPointF intersection;
	double r, s;

	if (index != -1) {
		for (int i = index - 1; i > 0; --i) {
			const auto& a1 = alist.at(i);
			const auto& a2 = alist.at(i + 1);

			QPointF p1(a1.position(), a1.height());
			QPointF p2(a2.position(), a2.height());

			bool intersect = iRIC::intersectionPoint(point, left, p1, p2, &intersection, &r, &s);
			if (! intersect) {continue;}

			*leftFound = true;
			*leftIndex = i;
			*leftXsec = intersection;
			break;
		}
	}

	*rightFound = false;
	for (int i = 0; i < alist.size(); ++i) {
		const auto& a = alist.at(i);
		if (a.position() > point.x()) {
			index = i - 1;
			break;
		}
	}

	if (index != -1) {
		for (int i = index; i < static_cast<int> (alist.size()) - 1; ++i) {
			const auto& a1 = alist.at(i);
			const auto& a2 = alist.at(i + 1);

			QPointF p1(a1.position(), a1.height());
			QPointF p2(a2.position(), a2.height());

			bool intersect = iRIC::intersectionPoint(point, right, p1, p2, &intersection, &r, &s);
			if (! intersect || r < 0 || r > 1 || s < 0 || s > 1) {continue;}

			*rightFound = true;
			*rightIndex = i + 1;
			*rightXsec = intersection;
			break;
		}
	}
}
