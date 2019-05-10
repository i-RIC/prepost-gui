#include "gridcreatingconditionlaplace.h"
#include "gridcreatingconditionlaplacesubregiondeploysettingdialog.h"
#include "ui_gridcreatingconditionlaplacesubregiondeploysettingdialog.h"

GridCreatingConditionLaplaceSubRegionDeploySettingDialog::GridCreatingConditionLaplaceSubRegionDeploySettingDialog(GridCreatingConditionLaplace* cond, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GridCreatingConditionLaplaceSubRegionDeploySettingDialog),
	m_condition {cond}
{
	ui->setupUi(this);

	connect(ui->autoRadioButton, SIGNAL(toggled(bool)), SLOT(requestUpdate()));
	connect(ui->streamwiseSlider, SIGNAL(valueChanged(int)), this, SLOT(requestUpdate()));
	connect(ui->crosssectionSlider, SIGNAL(valueChanged(int)), this, SLOT(requestUpdate()));
}

GridCreatingConditionLaplaceSubRegionDeploySettingDialog::~GridCreatingConditionLaplaceSubRegionDeploySettingDialog()
{
	delete ui;
	m_condition->hidePreviewGrid();
}

void GridCreatingConditionLaplaceSubRegionDeploySettingDialog::setParameter(GridCreatingConditionLaplace::DeployParameter param)
{
	m_oldParameter = param;

	if (param.setting == GridCreatingConditionLaplace::DeploySetting::Ratio) {
		ui->autoRadioButton->setChecked(true);
	} else {
		ui->manualRadioButton->setChecked(true);
		ui->streamwiseSlider->setValue(static_cast<int>(param.manualP / GridCreatingConditionLaplace::POISSONPARAM_FACTOR));
		ui->crosssectionSlider->setValue(static_cast<int>(param.manualQ / GridCreatingConditionLaplace::POISSONPARAM_FACTOR));
	}
	requestUpdate();
}

GridCreatingConditionLaplace::DeployParameter GridCreatingConditionLaplaceSubRegionDeploySettingDialog::parameter() const
{
	GridCreatingConditionLaplace::DeployParameter param;
	if (ui->autoRadioButton->isChecked()) {
		param.setting = GridCreatingConditionLaplace::DeploySetting::Ratio;
	} else {
		param.setting = GridCreatingConditionLaplace::DeploySetting::Poisson;
		param.manualP = ui->streamwiseSlider->value() * GridCreatingConditionLaplace::POISSONPARAM_FACTOR;
		param.manualQ = ui->crosssectionSlider->value() * GridCreatingConditionLaplace::POISSONPARAM_FACTOR;
	}
	return param;
}

void GridCreatingConditionLaplaceSubRegionDeploySettingDialog::accept()
{
	m_condition->updateDeployParameterForSelectedSubRegion(m_oldParameter);
	QDialog::accept();
}

void GridCreatingConditionLaplaceSubRegionDeploySettingDialog::reject()
{
	m_condition->updateDeployParameterForSelectedSubRegion(m_oldParameter);
	QDialog::reject();
}

void GridCreatingConditionLaplaceSubRegionDeploySettingDialog::requestUpdate()
{
	auto p = parameter();
	m_condition->updateDeployParameterForSelectedSubRegion(p);
	m_condition->requestPreviewSubRegionGrid();
}
