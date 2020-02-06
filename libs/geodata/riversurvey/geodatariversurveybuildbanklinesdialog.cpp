#include "geodatariversurveybuildbanklinesdialog.h"
#include "ui_geodatariversurveybuildbanklinesdialog.h"

GeoDataRiverSurveyBuildBankLinesDialog::GeoDataRiverSurveyBuildBankLinesDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GeoDataRiverSurveyBuildBankLinesDialog)
{
	ui->setupUi(this);
}

GeoDataRiverSurveyBuildBankLinesDialog::~GeoDataRiverSurveyBuildBankLinesDialog()
{
	delete ui;
}

double GeoDataRiverSurveyBuildBankLinesDialog::leftBankDistance()
{
	return ui->leftBankSpinBox->value();
}

void GeoDataRiverSurveyBuildBankLinesDialog::setLeftBankDistance(double dis)
{
	ui->leftBankSpinBox->setValue(dis);
}

double GeoDataRiverSurveyBuildBankLinesDialog::rightBankDistance()
{
	return ui->rightBankSpinBox->value();
}

void GeoDataRiverSurveyBuildBankLinesDialog::setRightBankDistance(double dis)
{
	ui->rightBankSpinBox->setValue(dis);
}
