#include "rawdatariversurveyimportersettingdialog.h"
#include "ui_rawdatariversurveyimportersettingdialog.h"

RawDataRiverSurveyImporterSettingDialog::RawDataRiverSurveyImporterSettingDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::RawDataRiverSurveyImporterSettingDialog)
{
	ui->setupUi(this);
}

RawDataRiverSurveyImporterSettingDialog::~RawDataRiverSurveyImporterSettingDialog()
{
	delete ui;
}

RawDataRiverSurveyImporterSettingDialog::CenterPointSetting RawDataRiverSurveyImporterSettingDialog::centerPointSetting() const
{
	if (ui->middlePointRadioButton->isChecked()){
		return cpMiddle;
	} else if (ui->smallestRadioButton->isChecked()){
		return cpElevation;
	}
	return cpMiddle;
}
