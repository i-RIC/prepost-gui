#include "ui_geodatariverpathpointrenamedialog.h"

#include "geodatariverpathpoint.h"
#include "geodatariverpathpointrenamedialog.h"
#include "geodatariversurvey.h"

#include <misc/iricundostack.h>

#include <QMessageBox>
#include <QUndoCommand>

#include <sstream>

GeoDataRiverPathPointRenameDialog::GeoDataRiverPathPointRenameDialog(GeoDataRiverPathPoint* p, GeoDataRiverSurvey* rs, QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GeoDataRiverPathPointRenameDialog)
{
	ui->setupUi(this);
	double current = p->name().toDouble();
	double min;
	double max;
	double small = 0.01;
	if (p->nextPoint() == nullptr) {
		min = current - 100;
	} else {
		min = p->nextPoint()->name().toDouble() + small;
	}
	if (p->previousPoint()->firstPoint()) {
		max = current + 100;
	} else {
		max = p->previousPoint()->name().toDouble() - small;
	}
	ui->nameSpinBox->setValue(current);
	ui->nameSpinBox->setMinimum(min);
	ui->nameSpinBox->setMaximum(max);
	m_point = p;
	m_rs = rs;
}

GeoDataRiverPathPointRenameDialog::~GeoDataRiverPathPointRenameDialog()
{
	delete ui;
}

class GeoDataRiverSurvey::RenameRiverPathPointCommand : public QUndoCommand
{
public:
	RenameRiverPathPointCommand(const QString& newname, GeoDataRiverPathPoint* p, GeoDataRiverSurvey* rs) :
		QUndoCommand{GeoDataRiverSurvey::tr("Rename Traversal Line")}
	{
		m_point = p;
		m_rs = rs;
		m_newName = newname;
		m_oldName = m_point->name();
	}
	void undo() {
		m_point->setName(m_oldName);
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
		m_rs->updateCrossectionWindows();
	}
	void redo() {
		m_point->setName(m_newName);
		m_rs->setModified();
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
		m_rs->updateCrossectionWindows();
	}
private:
	GeoDataRiverPathPoint* m_point;
	GeoDataRiverSurvey* m_rs;
	QString m_oldName;
	QString m_newName;
};

void GeoDataRiverPathPointRenameDialog::accept()
{
	std::ostringstream oss;
	oss << ui->nameSpinBox->value();
	QString newname = oss.str().c_str();

	if (newname != m_point->name()) {
		iRICUndoStack::instance().push(new GeoDataRiverSurvey::RenameRiverPathPointCommand(newname, m_point, m_rs));
	}
	QDialog::accept();
}
