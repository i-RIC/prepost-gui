#include "ui_geodatagdalnetcdfImportersettingdialog.h"

#include "geodatagdalnetcdfimportersettingdialog.h"

#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridattributedimension.h>

#include <QLabel>

GeoDataGdalNetcdfImporterSettingDialog::GeoDataGdalNetcdfImporterSettingDialog(QWidget* parent) :
	QDialog {parent},
	ui {new Ui::GeoDataGdalNetcdfImporterSettingDialog}
{
	ui->setupUi(this);
	connect(ui->valueComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleVarChange(int)));
}

GeoDataGdalNetcdfImporterSettingDialog::~GeoDataGdalNetcdfImporterSettingDialog()
{
	delete ui;
}

void GeoDataGdalNetcdfImporterSettingDialog::setCondition(SolverDefinitionGridAttribute* condition)
{
	auto& dims = condition->dimensions();
	for (int i = 0; i < dims.size(); ++i) {
		SolverDefinitionGridAttributeDimension* dim = dims.at(i);
		QLabel* label = new QLabel(this);
		label->setText(QString("%1: ").arg(dim->caption()));
		ui->dimGridLayout->addWidget(label, i, 0);
		QComboBox* comboBox = new QComboBox(this);
		ui->dimGridLayout->addWidget(comboBox, i, 1);
		m_dimComboBoxes.push_back(comboBox);
	}
}

void GeoDataGdalNetcdfImporterSettingDialog::setVariables(const std::vector<NcVariable>& vars)
{
	m_variables = vars;
	for (int i = 0; i < vars.size(); ++i) {
		const NcVariable& v = vars.at(i);
		ui->valueComboBox->addItem(v.name);
	}
	ui->valueComboBox->setCurrentIndex(0);
}

bool GeoDataGdalNetcdfImporterSettingDialog::needToShow() const
{
	if (m_variables.size() != 1) {
		return true;
	}
	const NcVariable& var = m_variables.at(0);
	if (m_dimComboBoxes.size() == 0) {
		return false;
	}
	if (m_dimComboBoxes.size() == 1 && var.dimensions.size() == 1) {
		return false;
	}
	return true;
}

QString GeoDataGdalNetcdfImporterSettingDialog::variableName() const
{
	int index = ui->valueComboBox->currentIndex();
	if (index == -1) {return "";}
	return m_variables.at(index).name;
}

std::vector<QString> GeoDataGdalNetcdfImporterSettingDialog::dimensionMappingSetting() const
{
	std::vector<QString> ret;
	int index = ui->valueComboBox->currentIndex();
	if (index == -1) {return ret;}
	const NcVariable& var = m_variables.at(index);

	for (int i = 0; i < m_dimComboBoxes.size(); ++i) {
		QComboBox* comboBox = m_dimComboBoxes.at(i);
		int current = comboBox->currentIndex();
		ret.push_back(var.dimensions.at(current));
	}
	return ret;
}

void GeoDataGdalNetcdfImporterSettingDialog::handleVarChange(int index)
{
	NcVariable v = m_variables.at(index);
	for (int i = 0; i < m_dimComboBoxes.size(); ++i) {
		QComboBox* comboBox = m_dimComboBoxes.at(i);
		comboBox->clear();
		for (int j = 0; j < v.dimensions.size(); ++j) {
			comboBox->addItem(v.dimensions.at(j));
		}
	}
}
