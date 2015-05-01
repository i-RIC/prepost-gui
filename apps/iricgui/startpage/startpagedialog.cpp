#include "ui_startpagedialog.h"

#include "startpagedialog.h"

#include <QPixmap>

StartPageDialog::StartPageDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::StartPageDialog)
{
	ui->setupUi(this);
	m_commandMode = cmNone;
	QPixmap logo(":/images/imgiRICLogo.png");
	ui->imageLabel->setPixmap(logo);

	ui->buttonBox->setFocus();
	connect(ui->startTab, SIGNAL(solverSelected(SolverDefinitionAbstract*)), this, SLOT(handleSolverSelection(SolverDefinitionAbstract*)));
	connect(ui->startTab, SIGNAL(projectFileSelected(QString)), this, SLOT(handleProjectFileSelection(QString)));
	connect(ui->startTab, SIGNAL(newProjectButtonClicked()), this, SLOT(handleNewProjectButtonClick()));
	connect(ui->startTab, SIGNAL(openProjectButtonClicked()), this, SLOT(handleOpenProjectButtonClick()));
}

StartPageDialog::~StartPageDialog()
{
	delete ui;
}

void StartPageDialog::handleNewProjectButtonClick()
{
	m_commandMode = cmNewOtherProject;
	accept();
}

void StartPageDialog::handleOpenProjectButtonClick()
{
	m_commandMode = cmOpenOtherProject;
	accept();
}

void StartPageDialog::handleProjectFileSelection(const QString& filename)
{
	m_commandMode = cmOpenProject;
	m_projectFileName = filename;
	accept();
}

void StartPageDialog::handleSolverSelection(SolverDefinitionAbstract* solverDef)
{
	m_commandMode = cmNewProject;
	m_solverDefinition = solverDef;
	accept();
}


void StartPageDialog::setSolverList(SolverDefinitionList* solverList)
{
	ui->startTab->setSolverList(solverList);
}

void StartPageDialog::setLocale(const QString& locale)
{
	ui->helpTab->setLocale(locale);
}
