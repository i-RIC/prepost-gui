#include "postcalculatedresult.h"
#include "postcalculatedresultargument.h"
#include "postcalculatedresultargumenteditdialog.h"
#include "postcalculatedresulteditdialog.h"
#include "postzonedatacontainer.h"
#include "ui_postcalculatedresulteditdialog.h"
#include "private/postcalculatedresulteditdialog_tabledelegate.h"

#include <QMessageBox>

#include <vtkCellData.h>
#include <vtkPointData.h>

#include <misc/stringtool.h>

PostCalculatedResultEditDialog::PostCalculatedResultEditDialog(QWidget *parent) :
	QDialog(parent),
	m_calculatedResult {new PostCalculatedResult(nullptr)},
	m_calculatedResultIndex {0},
	m_zoneDataContainer {nullptr},
	m_rejected {false},
	ui(new Ui::PostCalculatedResultEditDialog),
	m_tableDelegate {new TableDelegate (this)}
{
	ui->setupUi(this);
	ui->tableView->setModel(&m_model);
	ui->tableView->setItemDelegate(m_tableDelegate);
	m_tableDelegate->setCalculatedResult(m_calculatedResult);
	m_tableDelegate->setCalculatedResultIndex(m_calculatedResultIndex);

	connect(ui->addButton, SIGNAL(clicked()), this, SLOT(addArgument()));
	connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(deleteSelectedArgument()));
	connect(ui->testButton, SIGNAL(clicked()), this, SLOT(test()));
	connect(ui->positionComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handlePositionChange()));

	m_model.setColumnCount(3);
	QStringList labels;
	labels << tr("Result name") << tr("Variable name") << tr("Value for testing");
	m_model.setHorizontalHeaderLabels(labels);
}

PostCalculatedResultEditDialog::~PostCalculatedResultEditDialog()
{
	if (m_rejected) {
		delete m_calculatedResult;
	}
	delete ui;
}

void PostCalculatedResultEditDialog::setZoneDataContainer(PostZoneDataContainer* container)
{
	m_zoneDataContainer = container;
	m_tableDelegate->setZoneDataContainer(container);
	m_calculatedResult->setParent(container);
	ui->positionComboBox->clear();
	m_dataTypes.clear();

	if (container->data()->GetPointData()->GetNumberOfArrays() > 0) {
		ui->positionComboBox->addItem(tr("Grid node"));
		m_dataTypes.push_back(PostCalculatedResult::GridNode);
	}
	if (container->data()->GetCellData()->GetNumberOfArrays() > 0) {
		ui->positionComboBox->addItem(tr("Grid cell"));
		m_dataTypes.push_back(PostCalculatedResult::GridCell);
	}
	if (container->particleData() && container->particleData()->GetPointData()->GetNumberOfArrays() > 0) {
		ui->positionComboBox->addItem(tr("Particle"));
		m_dataTypes.push_back(PostCalculatedResult::Particle);
	}
	applySetting();
}

PostCalculatedResult* PostCalculatedResultEditDialog::calculatedResult() const
{
	return m_calculatedResult;
}

void PostCalculatedResultEditDialog::setCalculatedResult(PostCalculatedResult* result)
{
	m_calculatedResult->copyValue(*result);

	ui->nameEdit->setText(m_calculatedResult->name().c_str());
	ui->scriptEdit->setPlainText(m_calculatedResult->script());
	updateArgumentsTable();
}

void PostCalculatedResultEditDialog::setCalculatedResultIndex(int index)
{
	m_calculatedResultIndex = index;
	m_tableDelegate->setCalculatedResultIndex(index);
}

void PostCalculatedResultEditDialog::accept()
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

void PostCalculatedResultEditDialog::reject()
{
	m_rejected = true;
	QDialog::reject();
}

void PostCalculatedResultEditDialog::addArgument()
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

void PostCalculatedResultEditDialog::deleteSelectedArgument()
{
	int row = ui->tableView->selectionModel()->currentIndex().row();
	if (row < 0) {return;}

	auto name = m_model.data(m_model.index(row, 0)).toString();
	int result = QMessageBox::warning(this, tr("Warning"), tr("Are you sure you want to delete %1?").arg(name), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if (result == QMessageBox::No) {return;}

	m_model.removeRow(row);
}

void PostCalculatedResultEditDialog::handlePositionChange()
{
	m_calculatedResult->clearArguments();
	updateArgumentsTable();
	applySetting();
}

void PostCalculatedResultEditDialog::test()
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

void PostCalculatedResultEditDialog::applySetting()
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
		auto arg = new PostCalculatedResultArgument(m_calculatedResult);
		arg->setName(iRIC::toStr(m_model.data(m_model.index(i, 0)).toString()));
		arg->setRefName(iRIC::toStr(m_model.data(m_model.index(i, 1)).toString()));
		arg->setValueForTest(m_model.data(m_model.index(i, 2)).toDouble());
		m_calculatedResult->arguments().push_back(arg);
	}
}

bool PostCalculatedResultEditDialog::check()
{
	applySetting();
	return m_calculatedResult->testDefinition(this);
}

void PostCalculatedResultEditDialog::updateArgumentsTable()
{
	auto tableV = ui->tableView;

	const auto& args = m_calculatedResult->arguments();
	m_model.setRowCount(args.size());
	for (int i = 0; i < args.size(); ++i) {
		auto arg = args.at(i);
		m_model.setData(m_model.index(i, 0), arg->name().c_str());
		m_model.setData(m_model.index(i, 1), arg->refName().c_str());
		m_model.setData(m_model.index(i, 2), arg->valueForTest());
	}
}
