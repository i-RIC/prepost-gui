#include "mappingsettingdialog.h"
#include "ui_mappingsettingdialog.h"

#include "../data/project/project.h"

MappingSettingDialog::MappingSettingDialog(QWidget *parent) :
	QDialog(parent),
	m_project {nullptr},
	ui(new Ui::MappingSettingDialog)
{
	ui->setupUi(this);
}

MappingSettingDialog::~MappingSettingDialog()
{
	delete ui;
}

void MappingSettingDialog::setProject(Project* project)
{
	m_project = project;

	switch (project->mappingMethod()) {
	case Project::MappingMethod::AllMapToNearestCrossSection:
		ui->nearestRadioButton->setChecked(true);
		break;
	case Project::MappingMethod::TIN:
		ui->tinRadioButton->setChecked(true);
		break;
	case Project::MappingMethod::Template:
		ui->templateRadioButton->setChecked(true);
		break;
	}

	ui->resolutionSpinBox->setValue(project->templateMappingResolution());
	ui->streamLengthSpinBox->setValue(project->templateMappingStreamWiseLength());
	ui->crossLengthSpinBox->setValue(project->templateMappingCrossStreamWidth());
	ui->numExpansionsSpinBox->setValue(project->templateMappingNumberOfExpansions());
	ui->exponentSpinBox->setValue(project->templateMappingWeightExponent());
}

void MappingSettingDialog::accept()
{
	if (ui->nearestRadioButton->isChecked()) {
		m_project->setMappingMethod(Project::MappingMethod::AllMapToNearestCrossSection);
	} else if (ui->tinRadioButton->isChecked()) {
		m_project->setMappingMethod(Project::MappingMethod::TIN);
	} else {
		m_project->setMappingMethod(Project::MappingMethod::Template);
	}

	m_project->setTemplateMappingResolution(ui->resolutionSpinBox->value());
	m_project->setTemplateMappingStreamWiseLength(ui->streamLengthSpinBox->value());
	m_project->setTemplateMappingCrossStreamWidth(ui->crossLengthSpinBox->value());
	m_project->setTemplateMappingNumberOfExpansions(ui->numExpansionsSpinBox->value());
	m_project->setTemplateMappingWeightExponent(ui->exponentSpinBox->value());
}
