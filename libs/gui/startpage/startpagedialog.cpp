#include "ui_startpagedialog.h"

#include "startpagedialog.h"

#include "private/startpagedialog_impl.h"

#include <QPixmap>

StartPageDialog::StartPageDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::StartPageDialog),
	impl {new Impl {}}
{
	ui->setupUi(this);
	impl->m_commandMode = cmNone;
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
	delete impl;
}

StartPageDialog::CommandMode StartPageDialog::commandMode() const
{
	return impl->m_commandMode;
}

SolverDefinitionAbstract* StartPageDialog::solverDefinition() const
{
	return impl->m_solverDefinition;
}

const QString& StartPageDialog::projectFileName() const
{
	return impl->m_projectFileName;
}

void StartPageDialog::handleNewProjectButtonClick()
{
	impl->m_commandMode = cmNewOtherProject;
	accept();
}

void StartPageDialog::handleOpenProjectButtonClick()
{
	impl->m_commandMode = cmOpenOtherProject;
	accept();
}

void StartPageDialog::handleProjectFileSelection(const QString& filename)
{
	impl->m_commandMode = cmOpenProject;
	impl->m_projectFileName = filename;
	accept();
}

void StartPageDialog::handleSolverSelection(SolverDefinitionAbstract* solverDef)
{
	impl->m_commandMode = cmNewProject;
	impl->m_solverDefinition = solverDef;
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
