#include "preferencepagewebcsvtileadddialog.h"
#include "ui_preferencepagewebcsvtileadddialog.h"

#include <geodata/pointmap/geodatapointmapwebimportersetting.h>
#include <geodata/pointmap/geodatapointmapwebimportersettingmanager.h>

PreferencePageWebCsvTileAddDialog::PreferencePageWebCsvTileAddDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PreferencePageWebCsvTileAddDialog)
{
	ui->setupUi(this);
}

PreferencePageWebCsvTileAddDialog::~PreferencePageWebCsvTileAddDialog()
{
	delete ui;
}

GeoDataPointmapWebImporterSetting PreferencePageWebCsvTileAddDialog::setting() const
{
	GeoDataPointmapWebImporterSettingManager manager;
	return manager.setupSetting(ui->nameEdit->text(), ui->minZoomSpinBox->value(), ui->maxZoomSpinBox->value(), ui->urlEdit->text());
}
