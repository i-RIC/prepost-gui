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
	setTargets(targets);
}

void GeoDataRiverSurvey::CalcAreaConditionDialog::setTargets(const QStringList& targets)
{
	auto baseComboBox = ui->baseTargetComboBox;
	auto compareComboBox = ui->compareTargetComboBox;

	baseComboBox->clear();
	compareComboBox->clear();
	for (const auto& target : targets) {
		baseComboBox->addItem(target);
		compareComboBox->addItem(target);
	}
}

void GeoDataRiverSurvey::CalcAreaConditionDialog::setCompareTargetIndex(int index)
{
	auto baseComboBox = ui->baseTargetComboBox;
	auto compareComboBox = ui->compareTargetComboBox;

	compareComboBox->setCurrentIndex(index);

	// Set base combobox to the minimum index different from compare target index
	int baseIndex = (index == 0) ? 1 : 0;
	baseComboBox->setCurrentIndex(baseIndex);
}

int GeoDataRiverSurvey::CalcAreaConditionDialog::compareTargetIndex() const
{
	return ui->compareTargetComboBox->currentIndex();
}

int GeoDataRiverSurvey::CalcAreaConditionDialog::baseTargetIndex() const
{
	return ui->baseTargetComboBox->currentIndex();
}

QString GeoDataRiverSurvey::CalcAreaConditionDialog::filename() const
{
	return ui->filenameEdit->filename();
}

void GeoDataRiverSurvey::CalcAreaConditionDialog::setFilename(const QString& name)
{
	ui->filenameEdit->setFilename(name);
}

GeoDataRiverSurvey::CalcAreaConditionDialog::Mode GeoDataRiverSurvey::CalcAreaConditionDialog::mode() const
{
	auto index = ui->modeComboBox->currentIndex();
	if (index == 0) {
		return Mode::Each;
	} else if (index == 1) {
		return Mode::Statistic;
	}

	return Mode::Each;
}
