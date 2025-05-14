#include "../geodatarivercrosssection.h"
#include "../geodatariverpathpoint.h"

#include "geodatariversurvey_editcrosssectioncommand.h"
#include "geodatariversurveycrosssectionwindow_pointadddialog.h"
#include "ui_geodatariversurveycrosssectionwindow_pointadddialog.h"

#include <misc/iricundostack.h>

GeoDataRiverSurveyCrosssectionWindow::PointAddDialog::PointAddDialog(GeoDataRiverPathPoint* p, GeoDataRiverSurvey* rs, GeoDataRiverSurveyCrosssectionWindow *parent) :
	QDialog(parent),
	m_point {p},
	m_rs {rs},
	m_applyed {false},
	ui(new Ui::GeoDataRiverSurveyCrosssectionWindow_PointAddDialog)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);

	m_alist = m_point->crosssection().AltitudeInfo();
	m_odn = m_point->odn();

	connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &PointAddDialog::handleButtonClick);
}

GeoDataRiverSurveyCrosssectionWindow::PointAddDialog::~PointAddDialog()
{
	delete ui;
}

void GeoDataRiverSurveyCrosssectionWindow::PointAddDialog::accept()
{
	int pos;
	auto newlist = createNewList(&pos);

	iRICUndoStack::instance().push(new GeoDataRiverSurvey::EditCrosssectionCommand(false, tr("Add point"), m_point, newlist, pos, m_alist, GeoDataRiverSurvey::EditCrosssectionCommand::NO_SEL, m_odn, dynamic_cast<GeoDataRiverSurveyCrosssectionWindow*> (parentWidget()), m_rs));

	QDialog::accept();
}

void GeoDataRiverSurveyCrosssectionWindow::PointAddDialog::reject()
{
	if (m_applyed) {
		iRICUndoStack::instance().undo();
	}

	QDialog::reject();
}

void GeoDataRiverSurveyCrosssectionWindow::PointAddDialog::setPoint(const QPointF& position)
{
	if (! ui->clickRadioButton->isChecked()) {return;}

	ui->positionXEdit->setValue(position.x());
	ui->positionYEdit->setValue(position.y());

	apply();
}

void GeoDataRiverSurveyCrosssectionWindow::PointAddDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

void GeoDataRiverSurveyCrosssectionWindow::PointAddDialog::apply()
{
	int pos;
	auto newlist = createNewList(&pos);

	iRICUndoStack::instance().push(new GeoDataRiverSurvey::EditCrosssectionCommand(true, tr("Add point"), m_point, newlist, pos, m_alist, GeoDataRiverSurvey::EditCrosssectionCommand::NO_SEL, m_odn, dynamic_cast<GeoDataRiverSurveyCrosssectionWindow*> (parentWidget()), m_rs));

	m_applyed = true;
}

GeoDataRiverCrosssection::AltitudeList GeoDataRiverSurveyCrosssectionWindow::PointAddDialog::createNewList(int* insertPosition) const
{
	auto newlist = m_alist;
	GeoDataRiverCrosssection::Altitude newAlt(ui->positionXEdit->value() - m_point->crosssection().leftShift(), ui->positionYEdit->value());
	bool inserted = false;
	for (auto it = newlist.begin(); it != newlist.end(); ++it) {
		if (newAlt.position() < it->position()) {
			*insertPosition = it - newlist.begin();
			newlist.insert(it, newAlt);
			inserted = true;
			break;
		}
	}

	if (! inserted) {
		newlist.push_back(newAlt);
		*insertPosition = newlist.size() - 1;
	}

	return newlist;
}
