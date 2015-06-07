#include "ui_newprojectsolverselectingdialog.h"

#include "../solverdef/solverdefinitionabstractwidget.h"
#include "../solverdef/solverdefinitionlist.h"
#include "newprojectsolverselectingdialog.h"

#include <guicore/solverdef/solverdefinitionabstract.h>

NewProjectSolverSelectingDialog::NewProjectSolverSelectingDialog(SolverDefinitionList* list, QWidget* parent) :
	QDialog(parent),
	ui(new Ui::NewProjectSolverSelectingDialog)
{
	ui->setupUi(this);
	m_solverList = list;

	// setup widgets;
	QList<SolverDefinitionAbstract*> solvers  = list->solverList();
	for (SolverDefinitionAbstract* abst : solvers) {
		QString caption = abst->caption();
		ui->solverList->addItem(caption);
		SolverDefinitionAbstractWidget* w = new SolverDefinitionAbstractWidget(abst, this);
		ui->stackedWidget->addWidget(w);
	}
	connect(ui->solverList, SIGNAL(currentRowChanged(int)), ui->stackedWidget, SLOT(setCurrentIndex(int)));
	connect(ui->solverList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(listDoubleClicked()));
}

NewProjectSolverSelectingDialog::~NewProjectSolverSelectingDialog()
{
	delete ui;
}

SolverDefinitionAbstract* NewProjectSolverSelectingDialog::selectedSolver()
{
	return m_solverList->solverList().at(ui->solverList->currentIndex().row());
}

void NewProjectSolverSelectingDialog::setSolver(const QString& folderName)
{
	int i = 0;
	QList<SolverDefinitionAbstract*> solvers = m_solverList->solverList();
	for (SolverDefinitionAbstract* abst : solvers) {
		if (abst->folderName() == folderName) {
			ui->solverList->setCurrentRow(i);
			return;
		}
		++i;
	}
}

void NewProjectSolverSelectingDialog::listDoubleClicked()
{
	accept();
}
