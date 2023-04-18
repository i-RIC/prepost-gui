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

	QSettings settings;

	vtkTextPropertySettingContainer s_gridIndices;
	s_gridIndices.fontSize = 15;
	s_gridIndices.loadSetting(settings, "fontsetting/gridindices");
	ui->gridShapeWidget->setSetting(s_gridIndices);

	vtkTextPropertySettingContainer s_upstream;
	s_upstream.fontSize = 17;
	s_upstream.isBold = true;
	s_upstream.loadSetting(settings, "fontsetting/upstream");
	ui->upstreamWidget->setSetting(s_upstream);

	vtkTextPropertySettingContainer s_riverSurveyNames;
	s_riverSurveyNames.fontSize = 15;
	s_riverSurveyNames.loadSetting(settings, "fontsetting/riversurvey_names");
	ui->riverSurveyDataWidget->setSetting(s_riverSurveyNames);

	vtkTextPropertySettingContainer s_bcNames;
	s_bcNames.fontFamily = vtkTextPropertySettingContainer::ffArial;
	s_bcNames.fontSize = 14;
	s_bcNames.loadSetting(settings, "fontsetting/bc");
	ui->bcNamesWidget->setSetting(s_bcNames);

	QFont font;

	font.fromString(settings.value("fontsetting/vis_stringresult", QString("MS UI Gothic,9,-1,5,50,0,0,0,0,0")).toString());
	ui->visLabelsWidget->setFont(font);

	font.fromString(settings.value("fontsetting/chart_title", QString("MS UI Gothic,12,-1,5,75,0,0,0,0,0")).toString());
	ui->chartTitleWidget->setFont(font);

	font.fromString(settings.value("fontsetting/chart_legend", QString("MS UI Gothic,9,-1,5,50,0,0,0,0,0")).toString());
	ui->chartLegendWidget->setFont(font);

	font.fromString(settings.value("fontsetting/chart_axistitle", QString("MS UI Gothic,12,-1,5,75,0,0,0,0,0")).toString());
	ui->chartAxisTitleWidget->setFont(font);

	font.fromString(settings.value("fontsetting/chart_axistick", QString("MS UI Gothic,10,-1,5,50,0,0,0,0,0")).toString());
	ui->chartAxisTickWidget->setFont(font);

	// temporarily hidden
	ui->upstreamGroupBox->hide();
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

	s = ui->upstreamWidget->setting();
	s.saveSetting(&settings, "fontsetting/upstream");

	s = ui->riverSurveyDataWidget->setting();
	s.saveSetting(&settings, "fontsetting/riversurvey_names");

	s = ui->bcNamesWidget->setting();
	s.saveSetting(&settings, "fontsetting/bc");

	settings.setValue("fontsetting/vis_stringresult", ui->visLabelsWidget->font().toString());
	settings.setValue("fontsetting/chart_title", ui->chartTitleWidget->font().toString());
	settings.setValue("fontsetting/chart_legend", ui->chartLegendWidget->font().toString());
	settings.setValue("fontsetting/chart_axistitle", ui->chartAxisTitleWidget->font().toString());
	settings.setValue("fontsetting/chart_axistick", ui->chartAxisTickWidget->font().toString());
}
