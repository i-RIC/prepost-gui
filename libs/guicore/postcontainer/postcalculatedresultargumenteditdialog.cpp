#include "postcalculatedresult.h"
#include "postcalculatedresultargument.h"
#include "postcalculatedresultargumenteditdialog.h"
#include "postzonedatacontainer.h"
#include "ui_postcalculatedresultargumenteditdialog.h"

#include <misc/stringtool.h>

#include <QMessageBox>

#include <vtkCellData.h>
#include <vtkPointData.h>

PostCalculatedResultArgumentEditDialog::PostCalculatedResultArgumentEditDialog(QWidget *parent) :
	QDialog(parent),
	m_argument {new PostCalculatedResultArgument(nullptr)},
	m_calculatedResult {nullptr},
	m_calculatedResultIndex {0},
	m_zoneDataContainer {nullptr},
	m_rejected {false},
	ui(new Ui::PostCalculatedResultArgumentEditDialog)
{
	ui->setupUi(this);
}

PostCalculatedResultArgumentEditDialog::~PostCalculatedResultArgumentEditDialog()
{
	if (m_rejected) {
		delete m_argument;
	}
	delete ui;
}

PostCalculatedResultArgument* PostCalculatedResultArgumentEditDialog::argument() const
{
	m_argument->setName(iRIC::toStr(ui->resultComboBox->currentText()));
	m_argument->setRefName(iRIC::toStr(ui->variableNameEdit->text().trimmed()));
	m_argument->setValueForTest(ui->testValueSpinBox->value());

	return m_argument;
}

void PostCalculatedResultArgumentEditDialog::setArgument(PostCalculatedResultArgument* argument)
{
	m_argument->copyValue(*argument);

	ui->resultComboBox->setCurrentText(argument->name().c_str());
	ui->variableNameEdit->setText(argument->refName().c_str());
	ui->testValueSpinBox->setValue(argument->valueForTest());
}

void PostCalculatedResultArgumentEditDialog::setCalculatedResult(PostCalculatedResult* result)
{
	m_calculatedResult = result;
	m_argument->setParent(m_calculatedResult);
	setupComboBox();
}

void PostCalculatedResultArgumentEditDialog::setCalculatedResultIndex(int index)
{
	m_calculatedResultIndex = index;
	setupComboBox();
}

void PostCalculatedResultArgumentEditDialog::setZoneDataContainer(PostZoneDataContainer* container)
{
	m_zoneDataContainer = container;
	setupComboBox();
}

void PostCalculatedResultArgumentEditDialog::accept()
{
	auto vName = ui->variableNameEdit->text().trimmed();
	if (vName == "") {
		QMessageBox::warning(this, tr("Warning"), tr("Variable name is not input."));
		return;
	}
	for (auto arg : m_calculatedResult->arguments()) {
		if (vName == arg->refName().c_str()) {
			QMessageBox::warning(this, tr("Warning"), tr("This variable name is already used"));
			return;
		}
	}
	QDialog::accept();
}

void PostCalculatedResultArgumentEditDialog::reject()
{
	m_rejected = true;
	QDialog::reject();
}

void PostCalculatedResultArgumentEditDialog::setupComboBox()
{
	ui->resultComboBox->clear();
	if (m_zoneDataContainer == nullptr) {return;}
	if (m_calculatedResult == nullptr) {return;}

	vtkDataSetAttributes* attrs = nullptr;
	if (m_calculatedResult->dataType() == PostCalculatedResult::GridNode) {
		attrs = m_zoneDataContainer->data()->GetPointData();
	} else if (m_calculatedResult->dataType() == PostCalculatedResult::GridCell) {
		attrs = m_zoneDataContainer->data()->GetCellData();
	} else if (m_calculatedResult->dataType() == PostCalculatedResult::Particle) {
		attrs = m_zoneDataContainer->particleData()->GetPointData();
	}

	for (vtkIdType i = 0; i < attrs->GetNumberOfArrays(); ++i) {
		ui->resultComboBox->addItem(attrs->GetArrayName(i));
	}

	for (int i = 0; i < m_zoneDataContainer->calculatedResults().size(); ++i) {
		if (i == m_calculatedResultIndex) {break;}

		auto result = m_zoneDataContainer->calculatedResults().at(i);
		if (m_calculatedResult->dataType() != result->dataType()) {continue;}

		ui->resultComboBox->addItem(result->name().c_str());
	}
}
