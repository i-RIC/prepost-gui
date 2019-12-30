#include "geodatariversurveymlitimporter_problemsdialog.h"
#include "ui_geodatariversurveymlitimporter_problemsdialog.h"

GeoDataRiverSurveyMlitImporter::ProblemsDialog::ProblemsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GeoDataRiverSurveyMlitImporter_ProblemsDialog)
{
	ui->setupUi(this);
	ui->tableWidget->setColumnWidth(2, 300);
}

GeoDataRiverSurveyMlitImporter::ProblemsDialog::~ProblemsDialog()
{
	delete ui;
}

void GeoDataRiverSurveyMlitImporter::ProblemsDialog::setProblems(const std::vector<Problem>& problems)
{
	ui->tableWidget->setRowCount(problems.size());
	for (int i = 0; i < problems.size(); ++i) {
		const auto p = problems.at(i);
		auto w = new QTableWidgetItem(p.name);
		ui->tableWidget->setItem(i, 0, w);
		w = new QTableWidgetItem(p.relatedFile);
		ui->tableWidget->setItem(i, 1, w);
		w = new QTableWidgetItem(p.problem);
		ui->tableWidget->setItem(i, 2, w);
	}
}
