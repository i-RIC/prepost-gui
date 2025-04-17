#include "geodatariversurvey_calcareaconditiondialog.h"
#include "ui_geodatariversurvey_calcareaconditiondialog.h"

GeoDataRiverSurvey::CalcAreaConditionDialog::CalcAreaConditionDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GeoDataRiverSurvey_CalcAreaConditionDialog)
{
	ui->setupUi(this);
}

GeoDataRiverSurvey::CalcAreaConditionDialog::~CalcAreaConditionDialog()
{
	delete ui;
}

void GeoDataRiverSurvey::CalcAreaConditionDialog::setCompareTargets(const QStringList& targets)
{
	auto comboBox = ui->compareTargetComboBox;
	comboBox->clear();
	for (const auto& target : targets) {
		comboBox->addItem(target);
	}
}

int GeoDataRiverSurvey::CalcAreaConditionDialog::compareTargetIndex() const
{
	return ui->compareTargetComboBox->currentIndex();
}

QString GeoDataRiverSurvey::CalcAreaConditionDialog::filename() const
{
	return ui->filenameEdit->filename();
}

void GeoDataRiverSurvey::CalcAreaConditionDialog::setFilename(const QString& name)
{
	ui->filenameEdit->setFilename(name);
}
