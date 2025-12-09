#include "v4postcalculatedresult.h"
#include "v4postcalculatedresulteditdialog.h"
#include "v4postcalculatedresultlistdialog.h"
#include "ui_v4postcalculatedresultlistdialog.h"
#include "v4postzonedatacontainer.h"

#include <misc/errormessage.h>
#include <misc/projectlastiodirectory.h>

#include <QMessageBox>

v4PostCalculatedResultListDialog::v4PostCalculatedResultListDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::v4PostCalculatedResultListDialog)
{
	ui->setupUi(this);

	connect(ui->addButton, SIGNAL(clicked()), this, SLOT(add()));
	connect(ui->editButton, SIGNAL(clicked()), this, SLOT(editSelected()));
	connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(deleteSelected()));
	connect(ui->upButton, SIGNAL(clicked()), this, SLOT(moveUpSelected()));
	connect(ui->downButton, SIGNAL(clicked()), this, SLOT(moveDownSelected()));
	connect(ui->importButton, SIGNAL(clicked()), this, SLOT(importFromXML()));
	connect(ui->exportButton, SIGNAL(clicked()), this, SLOT(exportToXML()));
}

v4PostCalculatedResultListDialog::~v4PostCalculatedResultListDialog()
{
	delete ui;
}

void v4PostCalculatedResultListDialog::setZoneDataContainer(v4PostZoneDataContainer* container)
{
	m_zoneDataContainer = container;
	updateTable();
}

void v4PostCalculatedResultListDialog::reject()
{
	std::vector<v4PostCalculatedResult*> resultsBefore;
	bool allOK = true;
	for (auto r : m_zoneDataContainer->calculatedResults()) {
		allOK = allOK && r->checkArguments(resultsBefore, this);
		resultsBefore.push_back(r);
	}
	if (! allOK) {return;}

	QDialog::reject();
}

void v4PostCalculatedResultListDialog::add()
{
	v4PostCalculatedResultEditDialog dialog(this);
	dialog.setZoneDataContainer(m_zoneDataContainer);
	dialog.setCalculatedResultIndex(m_zoneDataContainer->calculatedResults().size());
	int result = dialog.exec();
	if (result == QDialog::Rejected) {return;}

	auto newResult = dialog.calculatedResult();
	m_zoneDataContainer->calculatedResults().push_back(newResult);

	updateTable();
	int newRow = static_cast<int> (m_zoneDataContainer->calculatedResults().size()) - 1;
	ui->tableWidget->setCurrentCell(newRow, 0);
}

void v4PostCalculatedResultListDialog::editSelected()
{
	int row = ui->tableWidget->currentRow();
	if (row < 0) {return;}

	v4PostCalculatedResultEditDialog dialog(this);
	dialog.setZoneDataContainer(m_zoneDataContainer);
	dialog.setCalculatedResultIndex(row);
	auto& results = m_zoneDataContainer->calculatedResults();
	auto result = results[row];
	results.erase(results.begin() + row);
	dialog.setCalculatedResult(result);
	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {
		results.insert(results.begin() + row, result);
		return;
	}

	auto newResult = dialog.calculatedResult();
	delete result;
	newResult->setParent(m_zoneDataContainer);
	results.insert(results.begin() + row, newResult);

	updateTable();
	ui->tableWidget->setCurrentCell(row, 0);
}

void v4PostCalculatedResultListDialog::deleteSelected()
{
	int row = ui->tableWidget->currentRow();
	if (row < 0) {return;}

	auto& results = m_zoneDataContainer->calculatedResults();
	auto result = results[row];

	int ret = QMessageBox::warning(this, tr("Warning"), tr("Are you sure you want to delete %1?").arg(result->name().c_str()), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if (ret == QMessageBox::No) {return;}

	delete result;
	results.erase(results.begin() + row);

	updateTable();
}

void v4PostCalculatedResultListDialog::moveUpSelected()
{
	int row = ui->tableWidget->currentRow();
	if (row <= 0) {return;}

	auto& results = m_zoneDataContainer->calculatedResults();
	auto movedResult = results[row];
	results.erase(results.begin() + row);
	results.insert(results.begin() + (row - 1), movedResult);

	updateTable();
	ui->tableWidget->setCurrentCell(row - 1, 0);
}

void v4PostCalculatedResultListDialog::moveDownSelected()
{
	int row = ui->tableWidget->currentRow();
	auto& results = m_zoneDataContainer->calculatedResults();

	if (row < 0 || row == results.size() - 1) {return;}

	auto movedResult = results[row];
	results.erase(results.begin() + row);
	results.insert(results.begin() + (row + 1), movedResult);

	updateTable();
	ui->tableWidget->setCurrentCell(row + 1, 0);
}

void v4PostCalculatedResultListDialog::importFromXML()
{
	QString dir = ProjectLastIODirectory::get();
	QString selectedFilter;
	v4PostCalculatedResultEditDialog dialog(this);
	QString filename = QFileDialog::getOpenFileName(this, tr("Select file to import"), dir, tr("XML files(*.xml)"));
	if (filename.isNull()) { return; }
	
	QFile f(filename);
	QDomDocument doc;
	QString errorStr;
	int errorLine;
	int errorColumn;
	QString errorHeader = "Error occured while loading %1\n";
	bool ok = doc.setContent(&f, &errorStr, &errorLine, &errorColumn);
	
	if (!ok) {
		QString msg = errorHeader;
		msg.append("Parse error %2 at line %3, column %4");
		msg = msg.arg(filename).arg(errorStr).arg(errorLine).arg(errorColumn);
		throw ErrorMessage(msg);
	}

	m_zoneDataContainer->loadFromProjectMainFile(doc.documentElement());
	updateTable();
}

void v4PostCalculatedResultListDialog::exportToXML()
{
	QString dir = ProjectLastIODirectory::get();
	QString selectedFilter;
	QString filename = QFileDialog::getSaveFileName(this, tr("Save XML file"), dir, tr("XML files(*.xml)"), &selectedFilter);
	if (filename.isNull()) { return; }
	QFile f(filename);
	f.open(QFile::WriteOnly);
	QXmlStreamWriter w(&f);
	w.setAutoFormatting(true);
	
	// start export xml
	auto calculatedResults = m_zoneDataContainer->calculatedResults();
	w.writeStartDocument("1.0");
	w.writeStartElement("Zone");
	for (auto calculatedResult : calculatedResults) {
		w.writeStartElement("SimpleOperationResult");
		calculatedResult->saveToProjectMainFile(w);
		w.writeEndElement();
	}
	w.writeEndElement();
	w.writeEndDocument();
	f.close();
}


void v4PostCalculatedResultListDialog::updateTable()
{
	ui->tableWidget->clearContents();

	auto& results = m_zoneDataContainer->calculatedResults();
	ui->tableWidget->setRowCount(static_cast<int> (results.size()));
	for (int i = 0; i < results.size(); ++i) {
		auto result = results.at(i);
		QTableWidgetItem* item = new QTableWidgetItem(result->name().c_str());
		ui->tableWidget->setItem(i, 0, item);
		QString type;
		switch (result->dataType()) {
		case v4PostCalculatedResult::GridNode:
			type = tr("Grid node");
			break;
		case v4PostCalculatedResult::GridCell:
			type = tr("Grid cell");
			break;
		case v4PostCalculatedResult::GridIEdge:
			type = tr("Grid iEdge");
			break;
		case v4PostCalculatedResult::GridJEdge:
			type = tr("Grid jEdge");
			break;
		case v4PostCalculatedResult::GridKEdge:
			type = tr("Grid kEdge");
			break;
		case v4PostCalculatedResult::Particle:
			type = tr("Particle");
			break;
		}
		item = new QTableWidgetItem(type);
		ui->tableWidget->setItem(i, 1, item);
	}
}
