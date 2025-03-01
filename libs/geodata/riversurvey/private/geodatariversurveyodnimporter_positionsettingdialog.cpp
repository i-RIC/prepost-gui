#include "geodatariversurveyodnimporter_positionsettingdialog.h"
#include "ui_geodatariversurveyodnimporter_positionsettingdialog.h"

GeoDataRiverSurveyOdnImporter::PositionSettingDialog::PositionSettingDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GeoDataRiverSurveyOdnImporter_PositionSettingDialog)
{
	ui->setupUi(this);
}

GeoDataRiverSurveyOdnImporter::PositionSettingDialog::~PositionSettingDialog()
{
	delete ui;
}

void GeoDataRiverSurveyOdnImporter::PositionSettingDialog::disableDistance()
{
	ui->nameRadioButton->setChecked(true);
	ui->distanceRadioButton->setDisabled(true);
}

GeoDataRiverSurveyOdnImporter::PositionSettingDialog::Position GeoDataRiverSurveyOdnImporter::PositionSettingDialog::position() const
{
	if (ui->distanceRadioButton->isChecked()) {
		return Position::Distance;
	} else {
		return Position::KPName;
	}
}
