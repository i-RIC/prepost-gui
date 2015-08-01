#include "ui_geodatanetcdfimportersettingdialog.h"

#include "geodatanetcdfimportersettingdialog.h"

#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridattributedimension.h>

#include <QLabel>

GeoDataNetcdfImporterSettingDialog::GeoDataNetcdfImporterSettingDialog(QWidget* parent) :
	QDialog {parent},
	ui {new Ui::GeoDataNetcdfImporterSettingDialog}
{
	ui->setupUi(this);
	connect(ui->valueComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleVarChange(int)));
}

GeoDataNetcdfImporterSettingDialog::~GeoDataNetcdfImporterSettingDialog()
{
	delete ui;
}

void GeoDataNetcdfImporterSettingDialog::setCondition(SolverDefinitionGridAttribute*	condition)
{
	QList<SolverDefinitionGridAttributeDimension*>& dims = condition->dimensions();
	for (int i = 0; i < dims.size(); ++i) {
		SolverDefinitionGridAttributeDimension* dim = dims.at(i);
		QLabel* label = new QLabel(this);
		label->setText(QString("%1: ").arg(dim->caption()));
		ui->dimGridLayout->addWidget(label, i, 0);
		QComboBox* comboBox = new QComboBox(this);
		ui->dimGridLayout->addWidget(comboBox, i, 1);
		m_dimComboBoxes.append(comboBox);
	}
}

void GeoDataNetcdfImporterSettingDialog::setVariables(const QList<NcVariable>& vars)
{
	m_variables = vars;
	for (int i = 0; i < vars.size(); ++i) {
		const NcVariable& v = vars.at(i);
		ui->valueComboBox->addItem(v.name);
	}
	ui->valueComboBox->setCurrentIndex(0);
}

bool GeoDataNetcdfImporterSettingDialog::needToShow() const
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

QString GeoDataNetcdfImporterSettingDialog::variableName() const
{
	int index = ui->valueComboBox->currentIndex();
	if (index == -1) {return "";}
	return m_variables.at(index).name;
}

QList<QString> GeoDataNetcdfImporterSettingDialog::dimensionMappingSetting() const
{
	QList<QString> ret;
	int index = ui->valueComboBox->currentIndex();
	if (index == -1) {return ret;}
	const NcVariable& var = m_variables.at(index);

	for (int i = 0; i < m_dimComboBoxes.size(); ++i) {
		QComboBox* comboBox = m_dimComboBoxes.at(i);
		int current = comboBox->currentIndex();
		ret.append(var.dimensions.at(current));
	}
	return ret;
}

void GeoDataNetcdfImporterSettingDialog::handleVarChange(int index)
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
