#include "preferencepageriversurveyfractionadddialog.h"
#include "ui_preferencepageriversurveyfractionadddialog.h"

PreferencePageRiverSurveyFractionAddDialog::PreferencePageRiverSurveyFractionAddDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PreferencePageRiverSurveyFractionAddDialog)
{
	ui->setupUi(this);
	ui->fractionSpinBox->selectAll();
}

PreferencePageRiverSurveyFractionAddDialog::~PreferencePageRiverSurveyFractionAddDialog()
{
	delete ui;
}

double PreferencePageRiverSurveyFractionAddDialog::fraction() const
{
	return ui->fractionSpinBox->value();
}
