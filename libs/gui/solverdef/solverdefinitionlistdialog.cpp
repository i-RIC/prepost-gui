#include "ui_solverdefinitionlistdialog.h"

#include "solverdefinitionabstractdialog.h"
#include "solverdefinitionlist.h"
#include "solverdefinitionlistdialog.h"

#include <guicore/solverdef/solverdefinitionabstract.h>
#include <misc/versionnumber.h>

#include <QList>
#include <QMessageBox>
#include <QTableWidgetItem>

SolverDefinitionListDialog::SolverDefinitionListDialog(const std::vector<SolverDefinitionAbstract*>& list, QWidget* parent) :
	QDialog(parent),
	ui(new Ui::SolverDefinitionListDialog),
	m_solverList (list)
{
	ui->setupUi(this);

	// initialize solver list
	connect(ui->solverListTable, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(handleCellDoubleClick(int, int)));
	connect(ui->showDetailButton, SIGNAL(clicked()), this, SLOT(showDetailOfCurrent()));

	setup();
}

SolverDefinitionListDialog::~SolverDefinitionListDialog()
{
	delete ui;
}

int SolverDefinitionListDialog::execToSelectSolver()
{
	setWindowTitle(tr("Select solver to open project"));
	ui->buttonBox->setStandardButtons(QDialogButtonBox::Ok);

	return exec();
}

int SolverDefinitionListDialog::selectedSolver() const
{
	return ui->solverListTable->currentRow();
}

void SolverDefinitionListDialog::changeEvent(QEvent* e)
{
	QDialog::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void SolverDefinitionListDialog::setup()
{
	QTableWidget* table = ui->solverListTable;

	table->clear();

	// setup basic properties
	table->setEditTriggers(QAbstractItemView::NoEditTriggers);
	table->setSortingEnabled(false);

	// set column headers
	table->setColumnCount(2);
	QStringList labels;
	labels << tr("Name") << tr("Version");
	table->setHorizontalHeaderLabels(labels);
	table->setColumnWidth(0, 200);
	table->setColumnWidth(1, 80);

	table->setRowCount(m_solverList.size());
	QTableWidgetItem* item;
	int i = 0;
	for (SolverDefinitionAbstract* abst : m_solverList) {
		item = new QTableWidgetItem(abst->caption());
		table->setItem(i, 0, item);
		item = new QTableWidgetItem(abst->version().toString());
		table->setItem(i, 1, item);
		table->setRowHeight(i, 18);
		/// hide vertical header
		table->takeVerticalHeaderItem(i);
		++i;
	}
}

void SolverDefinitionListDialog::handleCellDoubleClick(int row, int /*column*/)
{
	showDetail(row);
}

void SolverDefinitionListDialog::showDetailOfCurrent()
{
	QTableWidget* table = ui->solverListTable;
	showDetail(table->currentRow());
}

void SolverDefinitionListDialog::showDetail(int index)
{
	SolverDefinitionAbstract* abst = m_solverList.at(index);
	SolverDefinitionAbstractDialog dialog(abst, this);
	dialog.exec();
}
