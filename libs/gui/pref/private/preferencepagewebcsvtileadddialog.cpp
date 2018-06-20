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

void PreferencePageWebCsvTileAddDialog::setName(const QString& name)
{
	ui->nameEdit->setText(name);
}

void PreferencePageWebCsvTileAddDialog::setMinZoom(int zoom)
{
	ui->minZoomSpinBox->setValue(zoom);
}

void PreferencePageWebCsvTileAddDialog::setMaxZoom(int zoom)
{
	ui->maxZoomSpinBox->setValue(zoom);
}

void PreferencePageWebCsvTileAddDialog::setUrl(const QString& url)
{
	ui->urlEdit->setText(url);
}
