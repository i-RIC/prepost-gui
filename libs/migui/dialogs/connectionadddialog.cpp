#include "../project/connectioninput.h"
#include "../project/connectionoutput.h"
#include "../project/iricmiproject.h"
#include "../project/model.h"
#include "connectionadddialog.h"
#include "ui_connectionadddialog.h"

namespace {

bool isCompatibleWith(ConnectionInput* input, ConnectionOutput* output)
{
	if (input->valueType() != output->valueType()) {return false;}

	bool inputIsScalar = (input->inputType() != ConnectionInput::InputType::GridAttribute);
	bool outputIsScalar = (output->outputType() != ConnectionOutput::OutputType::GridAttribute);

	return (inputIsScalar == outputIsScalar);
}

bool hasCompatibleInput(Model* model, ConnectionOutput* output)
{
	for (auto input : model->inputs()) {
		if (isCompatibleWith(input, output)) {return true;}
	}

	return true;
}

} // namespace

ConnectionAddDialog::ConnectionAddDialog(iRICMIProject* project, QWidget *parent) :
	QDialog(parent),
	m_outputModel {nullptr},
	m_project {project},
	ui(new Ui::ConnectionAddDialog)
{
	ui->setupUi(this);

	connect<void (QComboBox::*)(int)>(ui->outputModelComboBox, &QComboBox::currentIndexChanged, this, &ConnectionAddDialog::handleOutputModelChange);
	connect<void (QComboBox::*)(int)>(ui->outputValueComboBox, &QComboBox::currentIndexChanged, this, &ConnectionAddDialog::handleOutputValueChange);
	connect<void (QComboBox::*)(int)>(ui->inputModelComboBox, &QComboBox::currentIndexChanged, this, &ConnectionAddDialog::handleInputModelChange);

	setupOutputModels();
}

ConnectionAddDialog::~ConnectionAddDialog()
{
	delete ui;
}

ConnectionOutput* ConnectionAddDialog::output() const
{
	return m_outputModel->outputs().at(ui->outputValueComboBox->currentIndex())->clone();
}

ConnectionInput* ConnectionAddDialog::input() const
{
	return m_inputValues.at(ui->inputValueComboBox->currentIndex())->clone();
}

void ConnectionAddDialog::handleOutputModelChange(int index)
{
	auto m = m_outputModels.at(index);
	m_outputModel = m;

	ui->outputValueComboBox->blockSignals(true);
	ui->outputValueComboBox->clear();
	for (auto output : m->outputs()) {
		ui->outputValueComboBox->addItem(output->caption());
	}
	ui->outputValueComboBox->blockSignals(false);

	handleOutputValueChange(0);
}

void ConnectionAddDialog::handleOutputValueChange(int index)
{
	auto output = m_outputModel->outputs().at(index);

	ui->inputModelComboBox->blockSignals(true);

	ui->inputModelComboBox->setEnabled(true);
	ui->inputValueComboBox->setEnabled(true);

	m_inputModels.clear();
	ui->inputModelComboBox->clear();
	for (auto model : m_project->models()) {
		if (hasCompatibleInput(model, output)) {
			ui->inputModelComboBox->addItem(model->name().c_str());
			m_inputModels.push_back(model);
		}
	}

	if (ui->inputModelComboBox->count() == 0) {
		ui->inputModelComboBox->setEnabled(false);
		ui->inputModelComboBox->addItem("No compatible input");

		ui->inputValueComboBox->setEnabled(false);
		ui->inputValueComboBox->clear();
		ui->inputValueComboBox->addItem("No compatible input");
	} else {
		handleInputModelChange(0);
	}

	ui->inputModelComboBox->blockSignals(false);
}

void ConnectionAddDialog::handleInputModelChange(int index)
{
	auto output = m_outputModel->outputs().at(ui->outputValueComboBox->currentIndex());

	auto inputModel = m_inputModels.at(index);

	ui->inputValueComboBox->clear();
	m_inputValues.clear();
	for (auto input : inputModel->inputs()) {
		if (isCompatibleWith(input, output)) {
			ui->inputValueComboBox->addItem(input->caption());
			m_inputValues.push_back(input);
		}
	}
}

void ConnectionAddDialog::setupOutputModels()
{
	ui->outputModelComboBox->blockSignals(true);
	for (auto m : m_project->models()) {
		if (m->outputs().size() == 0) {continue;}
		ui->outputModelComboBox->addItem(m->name().c_str());
		m_outputModels.push_back(m);
	}
	ui->outputModelComboBox->blockSignals(false);

	if (m_outputModels.size() != 0) {
		handleOutputModelChange(0);
	}
}
