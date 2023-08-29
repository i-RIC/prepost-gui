#include "ui_startpagedialog.h"
#include "../main/iricmainwindow.h"
#include "startpagedialog.h"
#include "private/startpagedialog_impl.h"

#include <guicore/solverdef/solverdefinitionabstract.h>
#include <misc/iricrootpath.h>

#include <QDir>
#include <QProcess>
#include <QPixmap>

StartPageDialog::StartPageDialog(iRICMainWindow* parent) :
	QDialog(parent),
	ui(new Ui::StartPageDialog),
	impl {new Impl {}}
{
	ui->setupUi(this);

	impl->m_mainWindow = parent;

	impl->m_commandMode = cmNone;
	QPixmap logo(":/images/imgiRICLogo.png");
	ui->imageLabel->setPixmap(logo);

	ui->buttonBox->setFocus();
	connect(ui->startTab, &StartPageLaunchTabWidget::solverSelected, this, &StartPageDialog::handleSolverSelection);
	connect(ui->startTab, &StartPageLaunchTabWidget::projectFileSelected, this, &StartPageDialog::handleProjectFileSelection);
	connect(ui->startTab, &StartPageLaunchTabWidget::newProjectButtonClicked, this, &StartPageDialog::handleNewProjectButtonClick);
	connect(ui->startTab, &StartPageLaunchTabWidget::openProjectButtonClicked, this, &StartPageDialog::handleOpenProjectButtonClick);
	connect(ui->launchMiGuiButton, &QPushButton::clicked, this, &StartPageDialog::handleLaunchMiGui);
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

void StartPageDialog::handleLaunchMiGui()
{
	QDir dir(iRICRootPath::get());
	auto program = dir.absoluteFilePath("IRICMI.exe");
	QStringList args;

	QProcess::startDetached(program, args);

	close();
	impl->m_mainWindow->close();
}

void StartPageDialog::setSolverList(SolverDefinitionList* solverList)
{
	ui->startTab->setSolverList(solverList);
}

void StartPageDialog::setLocale(const QString& locale)
{
	ui->helpTab->setLocale(locale);
}
