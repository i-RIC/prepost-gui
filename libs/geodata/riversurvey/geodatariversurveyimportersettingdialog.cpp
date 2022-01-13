#include "ui_geodatariversurveyimportersettingdialog.h"

#include "geodatariversurveyimportersettingdialog.h"

GeoDataRiverSurveyImporterSettingDialog::GeoDataRiverSurveyImporterSettingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GeoDataRiverSurveyImporterSettingDialog)
{
	ui->setupUi(this);
}

GeoDataRiverSurveyImporterSettingDialog::~GeoDataRiverSurveyImporterSettingDialog()
{
	delete ui;
}

void GeoDataRiverSurveyImporterSettingDialog::setWith4Points(bool with4points)
{
	if (with4points) {
		ui->middlePointRadioButton->setText(tr("Middle point of Low water way"));
		ui->smallestRadioButton->setText(tr("Where the elevation is the smallest in low water way"));
	}
}

void GeoDataRiverSurveyImporterSettingDialog::setAllNamesAreNumber(bool allNumber)
{
	if (allNumber) {
		ui->upToDownRadioButton->setChecked(false);
		ui->downToUpRadioButton->setChecked(true);
		ui->upToDownRadioButton->setEnabled(false);
		ui->downToUpRadioButton->setEnabled(false);
	} else {
		ui->upToDownRadioButton->setChecked(true);
	}
}

void GeoDataRiverSurveyImporterSettingDialog::setFileName(const QString& name)
{
	ui->csvFilenameEdit->setFilename(name + ".csv");
}

GeoDataRiverSurveyImporterSettingDialog::CenterPointSetting GeoDataRiverSurveyImporterSettingDialog::centerPointSetting() const
{
	if (ui->middlePointRadioButton->isChecked()) {
		return cpMiddle;
	} else if (ui->smallestRadioButton->isChecked()) {
		return cpElevation;
	} else if (ui->csvFileRadioButton->isChecked()) {
		return cpFile;
	}

	return cpMiddle;
}

bool GeoDataRiverSurveyImporterSettingDialog::reverseOrder() const
{
	return ui->downToUpRadioButton->isChecked();
}

QString GeoDataRiverSurveyImporterSettingDialog::csvFileName() const
{
	return ui->csvFilenameEdit->filename();
}
