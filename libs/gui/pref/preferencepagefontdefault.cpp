#include "ui_preferencepagefontdefault.h"

#include "preferencepagefontdefault.h"

#include <guibase/vtktextpropertysettingcontainer.h>

PreferencePageFontDefault::PreferencePageFontDefault(QWidget* parent) :
	PreferencePage(parent),
	ui(new Ui::PreferencePageFontDefault)
{
	ui->setupUi(this);

    vtkTextPropertySettingContainer s_global;
    s_global.loadSetting(m_settings, "global");
    ui->globalSettingWidget->setSetting(s_global);

    vtkTextPropertySettingContainer s_gridshape;
    s_gridshape.loadSetting(m_settings, "gridshape");
    ui->gridShapeWidget->setSetting(s_gridshape);

    vtkTextPropertySettingContainer s_scalarnode;
    s_scalarnode.loadSetting(m_settings, "scalarnode");
    ui->scalarNodeWidget->setSetting(s_scalarnode);

    vtkTextPropertySettingContainer s_arrow;
    s_arrow.loadSetting(m_settings, "arrow");
    ui->arrowWidget->setSetting(s_arrow);
}

PreferencePageFontDefault::~PreferencePageFontDefault()
{
	delete ui;
}

void PreferencePageFontDefault::update()
{
    bool useGlobalSetting = ui->globalSettingCheckBox->isChecked();

    vtkTextPropertySettingContainer s;

    if (useGlobalSetting) {
        s = ui->globalSettingWidget->setting();
    } else {
        s = ui->gridShapeWidget->setting();
    }
    s.saveSetting(&m_settings, "gridshape");
    s.saveSetting(&m_settings, "scalarnode");
    s.saveSetting(&m_settings, "arrow");
}
