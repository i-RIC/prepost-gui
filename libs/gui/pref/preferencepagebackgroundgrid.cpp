#include "preferencepagebackgroundgrid.h"
#include "ui_preferencepagebackgroundgrid.h"

PreferencePageBackgroundGrid::PreferencePageBackgroundGrid(QWidget *parent) :
	PreferencePage (parent),
	ui(new Ui::PreferencePageBackgroundGrid)
{
	ui->setupUi(this);
	m_settings.beginGroup("backgroundgrid");

	ui->csDirSpinBox->setValue(m_settings.value("rs_csdirdiv", 20).value<int>());
	ui->streamDirSpinBox->setValue(m_settings.value("rs_streamdirdiv", 4).value<int>());
}

PreferencePageBackgroundGrid::~PreferencePageBackgroundGrid()
{
	delete ui;
}

void PreferencePageBackgroundGrid::update()
{
	m_settings.setValue("rs_csdirdiv", ui->csDirSpinBox->value());
	m_settings.setValue("rs_streamdirdiv", ui->streamDirSpinBox->value());
}
