#include "rawdatariverpathpoint.h"
#include "rawdatariverpathpointrenamedialog.h"
#include "rawdatariversurvey.h"
#include "ui_rawdatariverpathpointrenamedialog.h"

#include <sstream>

#include <misc/iricundostack.h>

#include <QMessageBox>
#include <QUndoCommand>

RawDataRiverPathPointRenameDialog::RawDataRiverPathPointRenameDialog(RawDataRiverPathPoint* p, RawDataRiverSurvey* rs, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::RawDataRiverPathPointRenameDialog)
{
	ui->setupUi(this);
	double current = p->name().toDouble();
	double min;
	double max;
	double small = 0.01;
	if (p->nextPoint() == nullptr){
		min = current - 100;
	} else {
		min = p->nextPoint()->name().toDouble() + small;
	}
	if (p->previousPoint()->firstPoint()){
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

RawDataRiverPathPointRenameDialog::~RawDataRiverPathPointRenameDialog()
{
	delete ui;
}

class RawDataRiverPathPointRenameCommand : public QUndoCommand
{
public:
	RawDataRiverPathPointRenameCommand(const QString& newname, RawDataRiverPathPoint* p, RawDataRiverSurvey* rs)
		: QUndoCommand(RawDataRiverSurvey::tr("Rename Traversal Line"))
	{
		m_point = p;
		m_rs = rs;
		m_newName = newname;
		m_oldName = m_point->name();
	}
	void undo()
	{
		m_point->setName(m_oldName);
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
		m_rs->updateCrossectionWindows();
	}
	void redo()
	{
		m_point->setName(m_newName);
		m_rs->setModified();
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
		m_rs->updateCrossectionWindows();
	}
private:
	RawDataRiverPathPoint* m_point;
	RawDataRiverSurvey* m_rs;
	QString m_oldName;
	QString m_newName;
};

void RawDataRiverPathPointRenameDialog::accept()
{
	std::ostringstream oss;
	oss << ui->nameSpinBox->value();
	QString newname = oss.str().c_str();

	if (newname != m_point->name())
	{
		iRICUndoStack::instance().push(new RawDataRiverPathPointRenameCommand(newname, m_point, m_rs));
	}
	QDialog::accept();
}
