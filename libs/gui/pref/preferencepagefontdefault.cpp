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
	s_gridIndices.loadSetting(m_settings, "fontsetting/gridindices");
	s_gridIndices.fontSize = 15;
	ui->gridShapeWidget->setSetting(s_gridIndices);

	vtkTextPropertySettingContainer s_bcNames;
	s_bcNames.fontFamily = vtkTextPropertySettingContainer::ffArial;
	s_bcNames.fontSize = 14;
	s_bcNames.loadSetting(settings, "fontsetting/bc");
	ui->bcNamesWidget->setSetting(s_bcNames);

	vtkTextPropertySettingContainer s_arrows;
	s_arrows.fontSize = 10;
	s_arrows.loadSetting(m_settings, "fontsetting/arrow");
	ui->arrowWidget->setSetting(s_arrows);
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

	s = ui->bcNamesWidget->setting();
	s.saveSetting(&settings, "fontsetting/bc");

	s = ui->arrowWidget->setting();
	s.saveSetting(&settings, "fontsetting/arrow");
}
