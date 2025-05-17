#include "geodatariversurveyodnchecker_problemsdialog.h"
#include "ui_geodatariversurveyodnchecker_problemsdialog.h"

#include <QClipboard>

namespace {

const int NAME_WIDTH = 100;
const int MESSAGE_WIDTH = 400;

} // namespace

GeoDataRiverSurveyOdnChecker::ProblemsDialog::ProblemsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GeoDataRiverSurveyOdnChecker_ProblemsDialog)
{
	ui->setupUi(this);

	auto tw = ui->tableWidget;
	tw->setColumnWidth(0, NAME_WIDTH);
	tw->setColumnWidth(1, MESSAGE_WIDTH);

	connect(ui->copyToClipboardButton, &QPushButton::clicked, this, &ProblemsDialog::copyToClipboard);
}

GeoDataRiverSurveyOdnChecker::ProblemsDialog::~ProblemsDialog()
{
	delete ui;
}

void GeoDataRiverSurveyOdnChecker::ProblemsDialog::setProblems(const std::vector<Problem>& problems)
{
	m_problems = problems;

	ui->tableWidget->setRowCount(problems.size());
	for (int i = 0; i < static_cast<int> (problems.size()); ++i) {
		const auto& p = problems.at(i);

		auto name = new QTableWidgetItem(p.name);
		ui->tableWidget->setItem(i, 0, name);

		auto message = new QTableWidgetItem(p.message);
		ui->tableWidget->setItem(i, 1, message);
	}
}

void GeoDataRiverSurveyOdnChecker::ProblemsDialog::setForceFix(bool forceFix)
{
	if (! forceFix) {return;}

	ui->messageLabel->setText(tr("The problems above are found. Please fix them before continuing."));
	ui->buttonBox->removeButton(ui->buttonBox->button(QDialogButtonBox::Cancel));
}

void GeoDataRiverSurveyOdnChecker::ProblemsDialog::copyToClipboard()
{
	QString buffer;
	QTextStream stream(&buffer);
	for (const auto& p : m_problems) {
		stream << p.name << "\t" << p.message << "\n";
	}

	auto clipboard = QApplication::clipboard();
	clipboard->setText(buffer);

	QMessageBox::information(this, tr("Information"), tr("Problems are copied to clipboard."));
}
