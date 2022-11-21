#include "ui_preferencepagefontdefault.h"

#include "preferencepagefontdefault.h"

#include <guibase/vtktextpropertysettingcontainer.h>

#include <QSettings>

PreferencePageFontDefault::PreferencePageFontDefault(QWidget* parent) :
	PreferencePage(parent),
	ui(new Ui::PreferencePageFontDefault)
{
	ui->setupUi(this);
	ui->bcNamesWidget->hideColor();
	ui->arrowWidget->hideColor();
	ui->colorBarTitleWidget->hideSize();
	ui->colorBarLabelWidget->hideSize();

	QSettings settings;

	/*
	// not tested yet
	vtkTextPropertySettingContainer s_global;
	s_global.loadSetting(m_settings, "global");
	ui->globalSettingWidget->setSetting(s_global);


	vtkTextPropertySettingContainer s_scalarnode;
	s_scalarnode.loadSetting(m_settings, "scalarnode");
	ui->scalarNodeWidget->setSetting(s_scalarnode);
	*/

	// tested

	vtkTextPropertySettingContainer s_gridIndices;
	s_gridIndices.loadSetting(settings, "fontsetting/gridindices");
	s_gridIndices.fontSize = 15;
	ui->gridShapeWidget->setSetting(s_gridIndices);

	vtkTextPropertySettingContainer s_colorBarTitle;
	s_colorBarTitle.loadSetting(settings, "fontsetting/colorbar_title");
	ui->colorBarTitleWidget->setSetting(s_colorBarTitle);

	vtkTextPropertySettingContainer s_colorBarLabel;
	s_colorBarLabel.loadSetting(settings, "fontsetting/colorbar_label");
	ui->colorBarLabelWidget->setSetting(s_colorBarLabel);

	vtkTextPropertySettingContainer s_arrows;
	s_arrows.fontSize = 10;
	s_arrows.loadSetting(settings, "fontsetting/arrow");
	ui->arrowWidget->setSetting(s_arrows);

	vtkTextPropertySettingContainer s_gridCreatingCondition;
	s_gridCreatingCondition.fontSize = 17;
	s_gridCreatingCondition.isBold = true;
	s_gridCreatingCondition.loadSetting(settings, "fontsetting/gridcreatingcondition");
	ui->gridCreatingConditionWidget->setSetting(s_gridCreatingCondition);

	vtkTextPropertySettingContainer s_riverSurveyNames;
	s_riverSurveyNames.fontSize = 17;
	s_riverSurveyNames.loadSetting(settings, "fontsetting/riversurvey_names");
	ui->riverSurveyDataWidget->setSetting(s_riverSurveyNames);

	vtkTextPropertySettingContainer s_bcNames;
	s_bcNames.fontFamily = vtkTextPropertySettingContainer::ffArial;
	s_bcNames.fontSize = 14;
	s_bcNames.loadSetting(settings, "fontsetting/bc");
	ui->bcNamesWidget->setSetting(s_bcNames);
}

PreferencePageFontDefault::~PreferencePageFontDefault()
{
	delete ui;
}

void PreferencePageFontDefault::update()
{
	QSettings settings;

	// bool useGlobalSetting = ui->globalSettingCheckBox->isChecked();

	vtkTextPropertySettingContainer s;

	s = ui->gridShapeWidget->setting();
	s.saveSetting(&settings, "fontsetting/gridindices");

	s = ui->colorBarTitleWidget->setting();
	s.saveSetting(&settings, "fontsetting/colorbar_title");

	s = ui->colorBarLabelWidget->setting();
	s.saveSetting(&settings, "fontsetting/colorbar_label");

	s = ui->arrowWidget->setting();
	s.saveSetting(&settings, "fontsetting/arrow");

	s = ui->gridCreatingConditionWidget->setting();
	s.saveSetting(&settings, "fontsetting/gridcreatingcondition");

	s = ui->riverSurveyDataWidget->setting();
	s.saveSetting(&settings, "fontsetting/riversurvey_names");

	s = ui->bcNamesWidget->setting();
	s.saveSetting(&settings, "fontsetting/bc");
}
