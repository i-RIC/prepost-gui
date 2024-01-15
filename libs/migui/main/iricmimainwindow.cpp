#include "iricmimainwindow.h"
#include "ui_iricmimainwindow.h"
#include "dialogs/connectionadddialog.h"
#include "dialogs/existingmodeladddialog.h"
#include "dialogs/modeladddialog.h"
#include "dialogs/modeldeletedialog.h"
#include "private/iricmimainwindow_impl.h"
#include "../misc/recentprojectsmanager.h"
#include "../project/connection.h"
#include "../project/iricmiproject.h"
#include "../project/model.h"

#include <guicore/solverdef/solverdefinitionabstract.h>
#include <guicore/solverdef/solverdefinition.h>
#include <misc/iricrootpath.h>
#include <misc/filesystemfunction.h>
#include <misc/lastiodirectory.h>

#include <QCloseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QSettings>
#include <QStatusBar>

iRICMIMainWindow::iRICMIMainWindow(QWidget *parent) :
	QMainWindow(parent),
	impl {new Impl {this}},
	ui(new Ui::iRICMIMainWindow)
{
	ui->setupUi(this);
	impl->initViews();
	impl->resetViews(false);
	impl->makeConnections();

	QSettings settings;
	QString lastio = settings.value("general/lastiodir").toString();
	if (lastio == "" || ! QDir(lastio).exists()) {
		lastio = QDir::homePath();
	}
	LastIODirectory::set(lastio);
}

iRICMIMainWindow::~iRICMIMainWindow()
{
	closeProject();

	QSettings settings;
	settings.setValue("general/lastiodir", LastIODirectory::get());

	delete impl;
	delete ui;
}

const QLocale& iRICMIMainWindow::locale() const
{
	return impl->m_locale;
}

QTableView* iRICMIMainWindow::modelsTable() const
{
	return ui->modelsTableView;
}

QTableView* iRICMIMainWindow::connectionsTable() const
{
	return ui->connectionsTableView;
}

void iRICMIMainWindow::closeEvent(QCloseEvent* event)
{
	bool ok = impl->closeProject();
	if (! ok) {
		event->ignore();
	}

	QWidget::closeEvent(event);
}

void iRICMIMainWindow::newProject()
{
	if (! impl->closeProject()) {return;}

	bool ok = impl->newProject();
	if (! ok) {return;}

	impl->resetViews(true);
}

void iRICMIMainWindow::openProject()
{
	if (! impl->closeProject()) {return;}

	bool ok = impl->openProject();
	if (! ok) {return;}

	impl->resetViews(true);
}

void iRICMIMainWindow::saveProject()
{
	bool ok = impl->saveProject();
	if (! ok) {
		QMessageBox::critical(this, tr("Error"), tr("Saving %1 failed. Please make sure other programs are opening the file, and try again.").arg(QDir::toNativeSeparators(impl->m_project->fileName())));
		return;
	}

	statusBar()->showMessage(tr("Project successfully saved to %1").arg(QDir::toNativeSeparators(impl->m_project->fileName())), STATUSBAR_DISPLAYTIME);
}

void iRICMIMainWindow::closeProject()
{
	if (! impl->closeProject()) {return;}

	impl->resetViews(false);
}

void iRICMIMainWindow::reload()
{
	if (impl->m_project == nullptr) {return;}

	impl->m_project->reload(impl->m_solverDefinitionList);
}

void iRICMIMainWindow::addModel()
{
	ModelAddDialog dialog(impl->m_project, impl->m_solverDefinitionList, this);
	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	auto model = new Model(dialog.folderName(), dialog.solverDefinition(), impl->m_project);
	bool ok = model->createEmptyProjectData();
	if (! ok) {
		QMessageBox::critical(this, tr("Error"), tr("Error occured while preparing data for model in \"%1\"").arg(QDir::toNativeSeparators(model->absolutePath())));
		delete model;
		return;
	}

	auto def = new SolverDefinition(dialog.solverDefinition()->folder().absolutePath(), impl->m_locale);
	model->buildConnectionNodes(def);
	delete def;

	impl->m_project->addModel(model);
}

void iRICMIMainWindow::addExistingModels()
{
	auto models = ExistingModelAddDialog::buildModels(impl->m_project, impl->m_solverDefinitionList);
	if (models.size() == 0) {
		QMessageBox::warning(this, tr("Warning"), tr("No existing project folder found in \"%1\".").arg(QDir::toNativeSeparators(impl->m_project->dir().absolutePath())));
		return;
	}

	ExistingModelAddDialog dialog(this);
	dialog.setModels(models);

	int result = dialog.exec();
	if (result == QDialog::Rejected) {return;}

	for (auto m : dialog.models()) {
		auto def = new SolverDefinition(m->solverDefinition()->folder().absolutePath(), impl->m_locale);
		m->buildConnectionNodes(def);
		delete def;
		impl->m_project->addModel(m);
	}
}

void iRICMIMainWindow::removeModel()
{
	int row = modelsTable()->currentIndex().row();
	if (row < 0) {return;}

	auto model = impl->m_project->models().at(row);

	ModelDeleteDialog dialog(this);
	dialog.setFolder(model->absolutePath());
	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	if (dialog.deleteMode() == ModelDeleteDialog::DeleteMode::Delete) {
		iRIC::rmdirRecursively(model->absolutePath());
	}
	impl->m_project->removeModel(model);
}

void iRICMIMainWindow::addConnection()
{
	ConnectionAddDialog dialog(impl->m_project, this);
	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	auto output = dialog.output();
	auto input = dialog.input();
	auto con = new Connection(output, input);

	impl->m_project->addConnection(con);
}

void iRICMIMainWindow::removeConnection()
{
	int row = connectionsTable()->currentIndex().row();
	if (row < 0) {return;}

	int ret = QMessageBox::warning(this, tr("Warning"), tr("Are you sure you want to delete this connection?"), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
	if (ret == QMessageBox::Cancel) {return;}

	auto con = impl->m_project->connections().at(row);
	impl->m_project->removeConnection(con);
}

void iRICMIMainWindow::handleModelTableClick(const QModelIndex& index)
{
	if (index.column() != 3) {return;}

	auto model = impl->m_project->models().at(index.row());
	QDir modelDir(model->absolutePath());
	auto projectFolder = modelDir.absolutePath();

	QDir dir(iRICRootPath::get());
	auto program = dir.absoluteFilePath("iRIC.exe");

	QStringList args;
	args << projectFolder;
	QProcess::startDetached(program, args);
}

void iRICMIMainWindow::setupRecentProjectsMenu()
{
	QMenu* menu = ui->recentProjectsMenu;
	RecentProjectsManager::setupMenu(menu);

	for (auto action : menu->actions()) {
		connect(action, &QAction::triggered, this, &iRICMIMainWindow::openRecentProject);
	}
}

void iRICMIMainWindow::openRecentProject()
{
	auto action = qobject_cast<QAction*>(sender());
	if (action == nullptr) {return;}

	bool ok = impl->openProject(action->data().toString());
	if (! ok) {return;}

	impl->resetViews(true);
}

QStatusBar* iRICMIMainWindow::statusBar() const
{
	return ui->statusbar;
}
