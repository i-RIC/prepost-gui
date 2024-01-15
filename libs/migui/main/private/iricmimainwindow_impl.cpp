#include "iricmimainwindow_impl.h"
#include "iricmimainwindow_modelstabledelegate.h"
#include "../../project/iricmiproject.h"
#include "ui_iricmimainwindow.h"

#include <gui/solverdef/solverdefinitionlist.h>
#include <misc/filesystemfunction.h>
#include <misc/iricrootpath.h>
#include <misc/lastiodirectory.h>
#include <misc/stringtool.h>

#include <QDir>
#include <QFileInfo>
#include <QFileDialog>
#include <QLocale>
#include <QMessageBox>
#include <QSettings>

namespace {

const int MODEL_FOLDER_WIDTH = 100;
const int MODEL_SOLVER_WIDTH = 150;
const int MODEL_DESCRIPTION_WIDTH = 200;

} // namespace;

iRICMIMainWindow::Impl::Impl(iRICMIMainWindow* mainWindow) :
	m_project {nullptr},
	m_solverDefinitionList {nullptr},
	m_mainWindow {mainWindow}
{
	initLocale();
	setupSolverDefinitionList();
}

iRICMIMainWindow::Impl::~Impl()
{
	delete m_project;
	delete m_solverDefinitionList;
}

void iRICMIMainWindow::Impl::initViews()
{
	auto delegate = new ModelsTableDelegate(m_mainWindow);
	m_mainWindow->modelsTable()->setItemDelegate(delegate);
}

void iRICMIMainWindow::Impl::resetViews(bool enable)
{
	m_mainWindow->ui->tabWidget->setCurrentIndex(0);
	m_mainWindow->ui->tabWidget->setEnabled(enable);
}

void iRICMIMainWindow::Impl::makeConnections()
{
	auto w = m_mainWindow;
	connect(w->ui->newProjectAction, &QAction::triggered, w, &iRICMIMainWindow::newProject);
	connect(w->ui->openAction, &QAction::triggered, w, &iRICMIMainWindow::openProject);
	connect(w->ui->saveAction, &QAction::triggered, w, &iRICMIMainWindow::saveProject);
	connect(w->ui->closeAction, &QAction::triggered, w, &iRICMIMainWindow::closeProject);
	connect(w->ui->reloadAction, &QAction::triggered, w, &iRICMIMainWindow::reload);
	connect(w->ui->exitAction, &QAction::triggered, w, &iRICMIMainWindow::close);

	connect(w->ui->addModelButton, &QPushButton::clicked, w, &iRICMIMainWindow::addModel);
	connect(w->ui->addExistingModelButton, &QPushButton::clicked, w, &iRICMIMainWindow::addExistingModels);
	connect(w->ui->removeModelButton, &QPushButton::clicked, w, &iRICMIMainWindow::removeModel);
	connect(w->ui->addConnectionButton, &QPushButton::clicked, w, &iRICMIMainWindow::addConnection);
	connect(w->ui->removeConnectionButton, &QPushButton::clicked, w, &iRICMIMainWindow::removeConnection);

	connect(w->ui->modelsTableView, &QTableView::clicked, m_mainWindow, &iRICMIMainWindow::handleModelTableClick);
}

bool iRICMIMainWindow::Impl::newProject()
{
	QFileDialog dialog(m_mainWindow);
	dialog.setDirectory(LastIODirectory::get());
	dialog.setFileMode(QFileDialog::Directory);
	dialog.setOption(QFileDialog::ShowDirsOnly, true);
	dialog.setWindowTitle(tr("Select folder to save project"));

	while (true) {
		int ret = dialog.exec();
		if (ret != QDialog::Accepted) {return false;}

		QDir dir(dialog.selectedFiles().first());
		QFile projectFile(dir.absoluteFilePath("iricmi_project.xml"));
		if (projectFile.exists()) {
			QMessageBox::warning(m_mainWindow, tr("Warning"), tr("%1 already exists. Please select another folder.").arg(QDir::toNativeSeparators(projectFile.fileName())));
			continue;
		}
		m_project = new iRICMIProject(dir.absolutePath(), m_mainWindow);
		updateModelsColumnWidths();
		updateWindowTitle();
		return true;
	}
}

bool iRICMIMainWindow::Impl::openProject()
{
	auto name = QFileDialog::getOpenFileName(m_mainWindow, tr("Select project file to open"), LastIODirectory::get(),
															 tr("iRIC-MI project file (iricmi_project.xml)"));
	if (name.isEmpty()) {return false;}

	QFileInfo finfo(name);

	m_project = new iRICMIProject(finfo.absolutePath(), m_mainWindow);
	bool ok = m_project->load(m_solverDefinitionList, m_mainWindow);
	if (!ok) {return false;}

	updateModelsColumnWidths();
	updateWindowTitle();
	return true;
}

bool iRICMIMainWindow::Impl::saveProject()
{
	if (m_project == nullptr) {return false;}

	bool ok = m_project->save();
	if (! ok) {return false;}

	m_project->setModified(false);
	return true;
}

bool iRICMIMainWindow::Impl::closeProject()
{
	if (m_project == nullptr) {return true;}

	if (m_project->isModified()) {
		int ret = QMessageBox::warning(m_mainWindow, tr("Warning"), tr("The project is modified. Do you want to save?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Cancel);
		if (ret == QMessageBox::Cancel) {return false;}
		if (ret == QMessageBox::Yes) {
			bool ok = m_project->save();
			if (! ok) {
				QMessageBox::critical(m_mainWindow, tr("Error"), tr("Saving \"%1\" failed. Please make sure other program is opening the file, and try again.").arg(m_project->fileName()));
				return false;
			}
		}
	}
	delete m_project;
	m_project = nullptr;
	updateWindowTitle();
	return true;
}

void iRICMIMainWindow::Impl::initLocale()
{
	QSettings settings;
	auto loc = settings.value("general/locale", "").toString();
	if (loc == "") {
		m_locale = QLocale::system();
	} else {
		m_locale = QLocale(loc);
	}
}

void iRICMIMainWindow::Impl::setupSolverDefinitionList()
{
	QDir iricDir(iRICRootPath::get());
	iricDir.cdUp();
	iricDir.cdUp();
	auto publicSolvers = iricDir.absoluteFilePath("solvers");
	auto privateSolvers = iricDir.absoluteFilePath("private/solvers");

	m_solverDefinitionList = new SolverDefinitionList(publicSolvers, privateSolvers, m_locale, m_mainWindow);

}

void iRICMIMainWindow::Impl::updateModelsColumnWidths()
{
	auto table = m_mainWindow->ui->modelsTableView;
	table->setColumnWidth(0, MODEL_FOLDER_WIDTH);
	table->setColumnWidth(1, MODEL_SOLVER_WIDTH);
	table->setColumnWidth(2, MODEL_DESCRIPTION_WIDTH);
}

void iRICMIMainWindow::Impl::updateWindowTitle()
{
	auto appName = tr("iRIC-MI 0.0.1");
	if (m_project == nullptr) {
		m_mainWindow->setWindowTitle(appName);
	} else {
		m_mainWindow->setWindowTitle(QString("%1 %2").arg(appName).arg(QDir::toNativeSeparators(m_project->dir().absolutePath())));
	}
}
