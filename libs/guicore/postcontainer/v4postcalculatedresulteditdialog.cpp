#include "../solverdef/solverdefinitiongridtype.h"
#include "v4postcalculatedresult.h"
#include "v4postcalculatedresultargument.h"
#include "v4postcalculatedresultargumenteditdialog.h"
#include "v4postcalculatedresulteditdialog.h"
#include "v4postzonedatacontainer.h"
#include "ui_v4postcalculatedresulteditdialog.h"
#include "private/v4postcalculatedresulteditdialog_tabledelegate.h"

#include <QMessageBox>

#include <vtkCellData.h>
#include <vtkPointData.h>

#include <misc/stringtool.h>

v4PostCalculatedResultEditDialog::v4PostCalculatedResultEditDialog(QWidget *parent) :
	QDialog(parent),
	m_calculatedResult {new v4PostCalculatedResult(nullptr)},
	m_calculatedResultIndex {0},
	m_zoneDataContainer {nullptr},
	m_rejected {false},
	ui(new Ui::v4PostCalculatedResultEditDialog),
	m_tableDelegate {new TableDelegate (this)}
{
	ui->setupUi(this);
	ui->tableView->setModel(&m_model);
	ui->tableView->setItemDelegate(m_tableDelegate);
	m_tableDelegate->setCalculatedResult(m_calculatedResult);
	m_tableDelegate->setCalculatedResultIndex(m_calculatedResultIndex);

	connect(ui->addButton, &QPushButton::clicked, this, &v4PostCalculatedResultEditDialog::addArgument);
	connect(ui->deleteButton, &QPushButton::clicked, this, &v4PostCalculatedResultEditDialog::deleteSelectedArgument);
	connect(ui->testButton, &QPushButton::clicked, this, &v4PostCalculatedResultEditDialog::test);
	connect<void (QComboBox::*)(int)>(ui->positionComboBox, &QComboBox::currentIndexChanged, this, &v4PostCalculatedResultEditDialog::handlePositionChange);

	m_model.setColumnCount(3);
	QStringList labels;
	labels << v4PostCalculatedResultEditDialog::tr("Result name") << v4PostCalculatedResultEditDialog::tr("Variable name") << v4PostCalculatedResultEditDialog::tr("Value for testing");
	m_model.setHorizontalHeaderLabels(labels);
}

v4PostCalculatedResultEditDialog::~v4PostCalculatedResultEditDialog()
{
	if (m_rejected) {
		delete m_calculatedResult;
	}
	delete ui;
}

void v4PostCalculatedResultEditDialog::setZoneDataContainer(v4PostZoneDataContainer* container)
{
	m_zoneDataContainer = container;
	m_tableDelegate->setZoneDataContainer(container);
	m_calculatedResult->setParent(container);
	ui->positionComboBox->clear();
	m_dataTypes.clear();

	v4PostCalculatedResult tmpResult(container);

	tmpResult.setDataType(v4PostCalculatedResult::DataType::GridNode);
	if (tmpResult.dataSetAttributes()->GetNumberOfArrays() > 0) {
		ui->positionComboBox->addItem(tr("Grid node"));
		m_dataTypes.push_back(v4PostCalculatedResult::GridNode);
	}

	tmpResult.setDataType(v4PostCalculatedResult::DataType::GridCell);
	if (tmpResult.dataSetAttributes()->GetNumberOfArrays() > 0) {
		ui->positionComboBox->addItem(tr("Grid cell"));
		m_dataTypes.push_back(v4PostCalculatedResult::GridCell);
	}

	tmpResult.setDataType(v4PostCalculatedResult::DataType::GridIEdge);
	if (tmpResult.dataSetAttributes() != nullptr && tmpResult.dataSetAttributes()->GetNumberOfArrays() > 0) {
		ui->positionComboBox->addItem(tr("Grid iEdge"));
		m_dataTypes.push_back(v4PostCalculatedResult::GridIEdge);
	}

	tmpResult.setDataType(v4PostCalculatedResult::DataType::GridJEdge);
	if (tmpResult.dataSetAttributes() != nullptr && tmpResult.dataSetAttributes()->GetNumberOfArrays() > 0) {
		ui->positionComboBox->addItem(tr("Grid jEdge"));
		m_dataTypes.push_back(v4PostCalculatedResult::GridJEdge);
	}

	tmpResult.setDataType(v4PostCalculatedResult::DataType::GridKEdge);
	if (tmpResult.dataSetAttributes() != nullptr && tmpResult.dataSetAttributes()->GetNumberOfArrays() > 0) {
		ui->positionComboBox->addItem(tr("Grid kEdge"));
		m_dataTypes.push_back(v4PostCalculatedResult::GridKEdge);
	}

	tmpResult.setDataType(v4PostCalculatedResult::DataType::Particle);
	if (tmpResult.dataSetAttributes() != nullptr && tmpResult.dataSetAttributes()->GetNumberOfArrays() > 0) {
		ui->positionComboBox->addItem(tr("Particle"));
		m_dataTypes.push_back(v4PostCalculatedResult::Particle);
	}

	applySetting();
}

v4PostCalculatedResult* v4PostCalculatedResultEditDialog::calculatedResult() const
{
	return m_calculatedResult;
}

void v4PostCalculatedResultEditDialog::setCalculatedResult(v4PostCalculatedResult* result)
{
	m_calculatedResult->copyValue(*result);

	ui->nameEdit->setText(m_calculatedResult->name().c_str());
	ui->scriptEdit->setPlainText(m_calculatedResult->script());
	updateArgumentsTable();
}

void v4PostCalculatedResultEditDialog::setCalculatedResultIndex(int index)
{
	m_calculatedResultIndex = index;
	m_tableDelegate->setCalculatedResultIndex(index);
}

void v4PostCalculatedResultEditDialog::accept()
{
	auto name = ui->nameEdit->text().trimmed();
	if (name == "") {
		QMessageBox::warning(this, tr("Warning"), tr("Name is not input."));
		return;
	}
	for (auto r : m_zoneDataContainer->calculatedResults()) {
		if (name == r->name().c_str()) {
			QMessageBox::warning(this, tr("Warning"), tr("This name is already used"));
			return;
		}
	}
	if (! check()) {return;}

	QDialog::accept();
}

void v4PostCalculatedResultEditDialog::reject()
{
	m_rejected = true;
	QDialog::reject();
}

void v4PostCalculatedResultEditDialog::addArgument()
{
	m_model.setRowCount(m_model.rowCount() + 1);
	int lastRow = m_model.rowCount() - 1;
	auto atts = m_calculatedResult->dataSetAttributes();
	int arrayIndex = std::min(lastRow, atts->GetNumberOfArrays());
	QString name = atts->GetArrayName(arrayIndex);
	m_model.setData(m_model.index(lastRow, 0), name);
	m_model.setData(m_model.index(lastRow, 1), name.left(1));
	m_model.setData(m_model.index(lastRow, 2), 0);
}

void v4PostCalculatedResultEditDialog::deleteSelectedArgument()
{
	int row = ui->tableView->selectionModel()->currentIndex().row();
	if (row < 0) {return;}

	auto name = m_model.data(m_model.index(row, 0)).toString();
	int result = QMessageBox::warning(this, tr("Warning"), tr("Are you sure you want to delete %1?").arg(name), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if (result == QMessageBox::No) {return;}

	m_model.removeRow(row);
}

void v4PostCalculatedResultEditDialog::handlePositionChange()
{
	m_calculatedResult->clearArguments();
	updateArgumentsTable();
	applySetting();
}

void v4PostCalculatedResultEditDialog::test()
{
	applySetting();
	double value;
	bool ok = m_calculatedResult->testDefinition(this, &value);
	if (ok) {
		ui->testResultLabel->setText(QString::number(value));
	} else {
		ui->testResultLabel->setText("----");
	}
}

void v4PostCalculatedResultEditDialog::applySetting()
{
	m_calculatedResult->setName(iRIC::toStr(ui->nameEdit->text().trimmed()));
	int row = ui->positionComboBox->currentIndex();
	if (row < m_dataTypes.size()) {
		m_calculatedResult->setDataType(m_dataTypes.at(row));
	}
	// @todo support polydata name

	m_calculatedResult->setScript(ui->scriptEdit->toPlainText());
	m_calculatedResult->clearArguments();
	for (int i = 0; i < m_model.rowCount(); ++i) {
		auto arg = new v4PostCalculatedResultArgument(m_calculatedResult);
		arg->setName(iRIC::toStr(m_model.data(m_model.index(i, 0)).toString()));
		arg->setRefName(iRIC::toStr(m_model.data(m_model.index(i, 1)).toString()));
		arg->setValueForTest(m_model.data(m_model.index(i, 2)).toDouble());
		m_calculatedResult->arguments().push_back(arg);
	}
}

bool v4PostCalculatedResultEditDialog::check()
{
	applySetting();
	return m_calculatedResult->testDefinition(this);
}

void v4PostCalculatedResultEditDialog::updateArgumentsTable()
{
	auto gType = m_zoneDataContainer->gridType();
	const auto& args = m_calculatedResult->arguments();
	m_model.setRowCount(static_cast<int> (args.size()));
	for (int i = 0; i < args.size(); ++i) {
		auto arg = args.at(i);
		m_model.setData(m_model.index(i, 0), gType->outputCaption(arg->name()));
		m_model.setData(m_model.index(i, 1), arg->refName().c_str());
		m_model.setData(m_model.index(i, 2), arg->valueForTest());
	}
}
