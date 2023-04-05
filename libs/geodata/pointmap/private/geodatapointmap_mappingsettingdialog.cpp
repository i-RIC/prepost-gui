#include "geodatapointmap_mappingsetting.h"
#include "geodatapointmap_mappingsettingdialog.h"
#include "geodatapointmap_mappingsettingdialog_templatedetaildialog.h"
#include "ui_geodatapointmap_mappingsettingdialog.h"

GeoDataPointmap::MappingSettingDialog::MappingSettingDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GeoDataPointmap_MappingSettingDialog)
{
	ui->setupUi(this);
	connect(ui->detailButton, &QPushButton::clicked, [=]{openDetailDialog();});
}

GeoDataPointmap::MappingSettingDialog::~MappingSettingDialog()
{
	delete ui;
}

GeoDataPointmap::MappingSetting GeoDataPointmap::MappingSettingDialog::setting() const
{
	MappingSetting setting = m_setting;

	if (ui->tinRadioButton->isChecked()) {
		setting.mappingMode = MappingSetting::MappingMode::TIN;
	} else if (ui->templateRadioButton->isChecked()) {
		setting.mappingMode = MappingSetting::MappingMode::Template;
	} else if (ui->polygonRadioButton->isChecked()) {
		setting.mappingMode = MappingSetting::MappingMode::Polygons;
	}

	return setting;
}

void GeoDataPointmap::MappingSettingDialog::setSetting(const MappingSetting &setting)
{
	m_setting = setting;

	if (setting.mappingMode == MappingSetting::MappingMode::TIN) {
		ui->tinRadioButton->setChecked(true);
	} else if (setting.mappingMode == MappingSetting::MappingMode::Template) {
		ui->templateRadioButton->setChecked(true);
	} else if (setting.mappingMode == MappingSetting::MappingMode::Polygons) {
		ui->polygonRadioButton->setChecked(true);
	}
}

void GeoDataPointmap::MappingSettingDialog::openDetailDialog()
{
	TemplateDetailDialog dialog(this);
	dialog.setSetting(m_setting);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	m_setting = dialog.setting();
}
