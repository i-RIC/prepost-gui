#include "ui_geodatariverpathpointrenamedialog.h"

#include "geodatariverpathpoint.h"
#include "geodatariverpathpointrenamedialog.h"
#include "geodatariversurvey.h"
#include "private/geodatariversurvey_renameriverpathpointcommand.h"

#include <misc/iricundostack.h>

#include <QMessageBox>
#include <QRegExp>
#include <QRegExpValidator>
#include <QUndoCommand>

#include <sstream>

GeoDataRiverPathPointRenameDialog::GeoDataRiverPathPointRenameDialog(GeoDataRiverPathPoint* p, GeoDataRiverSurvey* rs, QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GeoDataRiverPathPointRenameDialog)
{
	ui->setupUi(this);

	QRegExp rx(GeoDataRiverPathPoint::NAME_REGEXP);
	auto validator = new QRegExpValidator(rx, this);
	ui->nameEdit->setValidator(validator);

	ui->nameEdit->setText(p->name());
	m_point = p;
	m_rs = rs;
}

GeoDataRiverPathPointRenameDialog::~GeoDataRiverPathPointRenameDialog()
{
	delete ui;
}

void GeoDataRiverPathPointRenameDialog::accept()
{
	QString newname = ui->nameEdit->text();

	if (newname != m_point->name()) {
		iRICUndoStack::instance().push(new GeoDataRiverSurvey::RenameRiverPathPointCommand(newname, m_point, m_rs));
	}
	QDialog::accept();
}
