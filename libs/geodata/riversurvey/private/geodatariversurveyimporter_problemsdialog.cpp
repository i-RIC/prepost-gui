#include "geodatariversurveyimporter_problemsdialog.h"
#include "ui_geodatariversurveyimporter_problemsdialog.h"

GeoDataRiverSurveyImporter::ProblemsDialog::ProblemsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GeoDataRiverSurveyImporter_ProblemsDialog)
{
	ui->setupUi(this);
	ui->tableWidget->setColumnWidth(1, 300);
}

GeoDataRiverSurveyImporter::ProblemsDialog::~ProblemsDialog()
{
	delete ui;
}

void GeoDataRiverSurveyImporter::ProblemsDialog::setProblems(const std::vector<Problem>& problems)
{
	ui->tableWidget->setRowCount(problems.size());
	for (int i = 0; i < problems.size(); ++i) {
		const auto p = problems.at(i);
		auto w = new QTableWidgetItem(p.name);
		ui->tableWidget->setItem(i, 0, w);
		w = new QTableWidgetItem(p.problem);
		ui->tableWidget->setItem(i, 1, w);
	}
}
