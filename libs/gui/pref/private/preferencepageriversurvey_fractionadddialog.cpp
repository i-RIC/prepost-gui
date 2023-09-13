#include "preferencepageriversurvey_fractionadddialog.h"
#include "ui_preferencepageriversurvey_fractionadddialog.h"

PreferencePageRiverSurvey::FractionAddDialog::FractionAddDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PreferencePageRiverSurvey_FractionAddDialog)
{
	ui->setupUi(this);
	ui->fractionSpinBox->selectAll();
}

PreferencePageRiverSurvey::FractionAddDialog::~FractionAddDialog()
{
	delete ui;
}

double PreferencePageRiverSurvey::FractionAddDialog::fraction() const
{
	return ui->fractionSpinBox->value();
}
