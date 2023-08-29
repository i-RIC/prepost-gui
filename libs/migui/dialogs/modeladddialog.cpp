#include "modeladddialog.h"
#include "ui_modeladddialog.h"
#include "../project/iricmiproject.h"

#include <gui/solverdef/solverdefinitionlist.h>
#include <gui/solverdef/solverdefinitionabstractwidget.h>
#include <guicore/solverdef/solverdefinitionabstract.h>
#include <misc/stringtool.h>

#include <QFileInfo>
#include <QTextCodec>

ModelAddDialog::ModelAddDialog(iRICMIProject* project, SolverDefinitionList* solvers, QWidget *parent) :
	QDialog(parent),
	m_solvers {solvers},
	m_project {project},
	ui(new Ui::ModelAddDialog)
{
	ui->setupUi(this);
	ui->splitter->setStretchFactor(0, 1);
	ui->splitter->setStretchFactor(1, 4);

	setupSolverList();
	checkInput();

	connect(ui->folderNameEdit, &QLineEdit::textChanged, this, &ModelAddDialog::checkInput);
	connect(ui->solverList, &QListWidget::currentRowChanged, this, &ModelAddDialog::checkInput);

	connect(ui->solverList, &QListWidget::currentRowChanged, ui->stackedWidget, &QStackedWidget::setCurrentIndex);
}

ModelAddDialog::~ModelAddDialog()
{
	delete ui;
}

std::string ModelAddDialog::folderName() const
{
	return iRIC::toStr(ui->folderNameEdit->text());
}

SolverDefinitionAbstract* ModelAddDialog::solverDefinition() const
{
	return m_solvers->solverList().at(ui->solverList->currentRow());
}

void ModelAddDialog::checkInput()
{
	bool ok = checkFolderName() && checkSolver();

	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ok);
}

void ModelAddDialog::setupSolverList()
{
	for (auto def : m_solvers->solverList()) {
		auto caption = def->caption();
		ui->solverList->addItem(caption);
		auto w = new SolverDefinitionAbstractWidget(def, this);
		ui->stackedWidget->addWidget(w);
	}

	ui->solverList->setCurrentRow(0);
}

bool ModelAddDialog::checkFolderName()
{
	auto name = ui->folderNameEdit->text();
	if (name == "") {
		ui->folderNameWarningLabel->setText(tr("Please input folder name."));
		return false;
	}
	auto asciiCodec = QTextCodec::codecForName("latin1");
	if (! asciiCodec->canEncode(name)) {
		ui->folderNameWarningLabel->setText(tr("You can use only ASCII characters."));
		return false;
	}

	auto fullname = m_project->dir().absoluteFilePath(name);
	QFileInfo finfo(fullname);
	if (finfo.exists()) {
		ui->folderNameWarningLabel->setText(tr("Folder \"%1\" already exists.").arg(QDir::toNativeSeparators(fullname)));
		return false;
	}
	ui->folderNameWarningLabel->setText("");

	return true;
}

bool ModelAddDialog::checkSolver()
{
	return (ui->solverList->currentRow() != -1);
}
