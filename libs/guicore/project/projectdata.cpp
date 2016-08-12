#include "../base/iricmainwindowinterface.h"
#include "../postcontainer/postsolutioninfo.h"
#include "../pre/base/preprocessorwindowinterface.h"
#include "../solverdef/solverdefinitiongridattribute.h"
#include "../solverdef/solverdefinitiongridtype.h"
#include "cgnsfileentry.h"
#include "cgnsfilelist.h"
#include "projectdata.h"
#include "projectmainfile.h"
#include "projectworkspace.h"
#include "private/projectdatacopythread.h"
#include "private/projectdatamovethread.h"

#include <guibase/executer/executerwatcher.h>
#include <guibase/executer/silentexecuterwatcher.h>
#include <guibase/executer/threadexecuter.h>
#include <guibase/executer/ziparchiveprocessexecuter.h>
#include <guibase/executer/unziparchiveprocessexecuter.h>
#include <misc/filesystemfunction.h>
#include <misc/versionnumber.h>
#include <misc/ziparchive.h>

#include <memory>

#include <QCoreApplication>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QProcess>
#include <QTime>
#include <QUrl>

namespace {

} // namespace

const QString ProjectData::LOCKFILENAME = "lock";

ProjectData::ProjectData(const QString& workdir, iRICMainWindowInterface* parent) :
	QObject(parent),
	m_mainWindow {parent},
	m_workDirectory {workdir},
	m_solverDefinition {nullptr},
	m_folderProject {false},
	m_isPostOnlyMode {false},
	m_lockFile {nullptr},
	m_mainfile {new ProjectMainFile(this)}
{
	// if the workdirectory doesn't exists, make it.
	QDir wdir(m_workDirectory);
	if (! wdir.exists()) {
		QDir parent = wdir;
		parent.cdUp();
		parent.mkdir(wdir.dirName());
	}
	lock();
	connect(m_mainfile, SIGNAL(cgnsFileSwitched()), parent, SLOT(handleCgnsSwitch()));

	if (parent == nullptr) {
		// this project data should not attatched to mainwindow.
		return;
	}
	parent->setProjectData(this);
}

ProjectData::~ProjectData()
{
	delete m_solverDefinition;
	delete m_mainfile;
	// close lock file
	unlock();

	// remove the workdirectory recursively, if it is under workspace.
	if (isInWorkspace()) {
		iRIC::rmdirRecursively(m_workDirectory);
	}
}

QString ProjectData::absoluteFileName(const QString& relativeFileName) const
{
	QDir workdir(m_workDirectory);
	return workdir.absoluteFilePath(relativeFileName);
}

QString ProjectData::workDirectory() const
{
	return m_workDirectory;
}

QString ProjectData::filename() const
{
	return m_filename;
}

void ProjectData::setFilename(const QString& fname, bool isFolder)
{
	m_filename = fname;
	m_folderProject = isFolder;
}

void ProjectData::setPostOnlyMode()
{
	m_isPostOnlyMode = true;
}

bool ProjectData::isPostOnlyMode() const
{
	return m_isPostOnlyMode;
}

bool ProjectData::unzipFrom(const QString& filename)
{
	auto executer = new UnzipArchiveProcessExecuter(filename, workDirectory());
	auto mw = mainWindow();

	std::unique_ptr<ExecuterWatcher> watcher;
	if (mw == nullptr) {
		watcher = std::unique_ptr<ExecuterWatcher> {new SilentExecuterWatcher(executer)};
	} else {
		watcher = std::unique_ptr<ExecuterWatcher> {mw->buildExecuteWatcher(executer)};
	}
	watcher->setMessage(tr("Loading project file..."));
	watcher->execute();

	if (executer->isCanceled()) {return false;}

	m_filename = filename;
	return true;
}

void ProjectData::loadSolverInformation()
{
	// load project.xml. (only solver name and version)
	mainfile()->loadSolverInformation();
}

void ProjectData::setSolverDefinition(SolverDefinition* def)
{
	m_solverDefinition = def;
	// check solver definition.
	checkGridConditions();
	initForSolverDefinition();
}

void ProjectData::initForSolverDefinition()
{
	mainfile()->initForSolverDefinition();
	mainWindow()->initForSolverDefinition();
}

void ProjectData::load()
{
	// load project.xml.
	mainfile()->load();
}

void ProjectData::loadCgnsList()
{
	mainfile()->loadCgnsList();
}

bool ProjectData::save()
{
	mainWindow()->enterModelessDialogMode();
	qApp->processEvents();
	mainWindow()->setCursor(Qt::WaitCursor);
	bool ret = mainfile()->save();
	mainWindow()->setCursor(Qt::ArrowCursor);
	mainWindow()->exitModelessDialogMode();
	return ret;
}

ProjectMainFile* ProjectData::mainfile() const
{
	return m_mainfile;
}

SolverDefinition* ProjectData::solverDefinition() const
{
	return m_solverDefinition;
}

bool ProjectData::zipTo(const QString& filename)
{
	QStringList filelist;
	QDir workdir(workDirectory());
	filelist << workdir.relativeFilePath(mainfile()->filename());
	filelist << mainfile()->containedFiles();

	// define temporary file used as output file of zip process.
	QString outfile = m_mainWindow->workspace()->tmpFileName();
	outfile.append(".zip");

	auto executer = new ZipArchiveProcessExecuter(outfile, workDirectory(), filelist);
	auto mw = mainWindow();

	std::unique_ptr<ExecuterWatcher> watcher;
	if (mw == nullptr) {
		watcher = std::unique_ptr<ExecuterWatcher> {new SilentExecuterWatcher(executer)};
	} else {
		watcher = std::unique_ptr<ExecuterWatcher> {mw->buildExecuteWatcher(executer)};
	}
	watcher->setMessage(tr("Saving project file..."));
	watcher->execute();

	if (executer->isCanceled()) {
		mainWindow()->workspace()->removeZipTrashes();
		return false;
	}
	if (QFile::exists(filename)) {
		bool ok = QFile::remove(filename);
		if (! ok) {
			// unable to remove the file.
			QMessageBox::critical(mainWindow(), tr("Error"), tr("Could not overwrite %1.").arg(QDir::toNativeSeparators(filename)));
			QFile::remove(outfile);
			return false;
		}
	}

	// rename the temporary file into the target iRIC project file.
	QFile::rename(outfile, filename);

	// save the filename.
	m_filename = filename;
	m_folderProject = false;
	return true;
}

QString ProjectData::newWorkfolderName(const QDir& workspace)
{
	return iRIC::getTempFileName(workspace.absolutePath());
}

bool ProjectData::switchToDefaultCgnsFile()
{
	QString current = m_mainfile->cgnsFileList()->current()->filename();
	return m_mainfile->switchCgnsFile(current);
}

QString ProjectData::workCgnsFileName(const QString& name) const
{
	QString tmpstr = name;
	return QDir(m_workDirectory).absoluteFilePath(tmpstr.append(".cgn"));
}

QString ProjectData::currentCgnsFileName() const
{
	QString filename = m_mainfile->cgnsFileList()->current()->filename();
	return workCgnsFileName(filename);
}

void ProjectData::openWorkDirectory()
{
	QUrl url(QString("file:///").append(m_workDirectory));
	QDesktopServices::openUrl(url);
}

void ProjectData::setVersion(const VersionNumber& v)
{
	m_mainfile->setIRICVersion(v);
}

const VersionNumber ProjectData::version()
{
	return m_mainfile->iRICVersion();
}

QString ProjectData::tmpFileName() const
{
	return iRIC::getTempFileName(m_workDirectory);
}

void ProjectData::checkGridConditions()
{
	QStringList ngtypes;
	const QList<SolverDefinitionGridType*> gtypes = m_solverDefinition->gridTypes();
	for (auto it = gtypes.begin(); it != gtypes.end(); ++it) {
		const SolverDefinitionGridType* gt = *it;
		bool ok = false;
		ok = (gt->defaultGridType() != SolverDefinitionGridType::gtStructured2DGrid);
		const QList<SolverDefinitionGridAttribute*>& conds = gt->gridAttributes();
		for (auto cit = conds.begin(); cit != conds.end(); ++cit) {
			const SolverDefinitionGridAttribute* cond = *cit;
			ok = ok || (cond->name() == "Elevation");
		}
		if (! ok) {
			ngtypes.append(gt->caption());
		}
	}
	if (! ngtypes.empty() && m_mainWindow) {
		// NG grid types were found.
		QMessageBox::warning(
			m_mainWindow, tr("Warning"),
			tr("Elevation grid attribute is not defined for grid type %1. Grid I/O functions, grid creating functions may not work correnctly for these grid types.").arg(ngtypes.join(", ")));
	}
}

bool ProjectData::lock()
{
	unlock();
	m_lockFile = new QFile(lockFileName(), this);
	bool ok = m_lockFile->open(QIODevice::WriteOnly);
	if (! ok) {
		delete m_lockFile;
		m_lockFile = nullptr;
		return false;
	}
	return true;
}

void ProjectData::unlock()
{
	if (m_lockFile == nullptr) {return;}
	m_lockFile->close();
	delete m_lockFile;
	QFile::remove(lockFileName());
	m_lockFile = nullptr;
}

QString ProjectData::lockFileName() const
{
	return QDir(m_workDirectory).absoluteFilePath(ProjectData::LOCKFILENAME);
}

bool ProjectData::moveTo(const QString& newWorkFolder)
{
	if (QFile::exists(newWorkFolder)) {
		if (! iRIC::rmdirRecursively(newWorkFolder)) {
			return false;
		}
		qApp->processEvents();
		QFileInfo finfo(newWorkFolder);
		QDir dir(finfo.absolutePath());
		if (! dir.mkdir(finfo.fileName())) {
			return false;
		}
		qApp->processEvents();
	}
#ifdef Q_OS_WIN32
	QString oldDrive = m_workDirectory.left(1).toUpper();
	QString newDrive = newWorkFolder.left(1).toUpper();
	if (oldDrive != newDrive) {
		// moving is done between different drives.
		// execute copy and remove.
		QString currentWorkDirectory = m_workDirectory;
		bool ok = copyTo(newWorkFolder, true);
		if (! ok) {return false;}
		ok = iRIC::rmdirRecursively(currentWorkDirectory);
		// return true, even if rmdir failed.
		return true;
	}
#endif // Q_OS_WIN32
	// move current folder to new folder.
	unlock();
	mainfile()->postSolutionInfo()->close();

	ProjectDataMoveThread* thread = new ProjectDataMoveThread(m_workDirectory, newWorkFolder, this);
	ThreadExecuter* executer = new ThreadExecuter(thread);
	auto mw = mainWindow();

	std::unique_ptr<ExecuterWatcher> watcher;
	if (mw == nullptr) {
		watcher = std::unique_ptr<ExecuterWatcher> {new SilentExecuterWatcher(executer)};
	} else {
		watcher = std::unique_ptr<ExecuterWatcher> {mw->buildExecuteWatcher(executer)};
	}
	watcher->setMessage(tr("Saving project..."));
	watcher->execute();

	if (executer->isCanceled()) {
		lock();
		return false;
	}

	if (! thread->result()) {
		// moving failed. keep the current workspace.
		lock();
		return false;
	}
	// moving succeeded.
	m_workDirectory = newWorkFolder;
	return lock();
}

bool ProjectData::copyTo(const QString& newWorkFolder, bool switchToNewFolder)
{
	if (QFile::exists(newWorkFolder)) {
		QFileInfo finfo(newWorkFolder);
		QString parentPath = finfo.absolutePath();
		QDir dir(parentPath);
		QString dirName = finfo.fileName();
		if (! iRIC::rmdirRecursively(newWorkFolder)) {
			return false;
		}
		qApp->processEvents();
		if (! dir.mkdir(dirName)) {
			return false;
		}
		qApp->processEvents();
	}

	// copy current folder to new folder.
	unlock();
	mainfile()->postSolutionInfo()->close();

	ProjectDataCopyThread* thread = new ProjectDataCopyThread(m_workDirectory, newWorkFolder, this);
	ThreadExecuter* executer = new ThreadExecuter(thread);
	auto mw = mainWindow();

	std::unique_ptr<ExecuterWatcher> watcher;
	if (mw == nullptr) {
		watcher = std::unique_ptr<ExecuterWatcher> {new SilentExecuterWatcher(executer)};
	} else {
		watcher = std::unique_ptr<ExecuterWatcher> {mw->buildExecuteWatcher(executer)};
	}
	watcher->setMessage(tr("Saving project..."));
	watcher->execute();

	if (executer->isCanceled()) {
		goto ERROR;
	}
	if (! thread->result()) {
		goto ERROR;
	}

	// copying succeeded.
	if (switchToNewFolder) {
		m_workDirectory = newWorkFolder;
	}
	return lock();

ERROR:
	iRIC::rmdirRecursively(newWorkFolder);
	lock();
	return false;
}

bool ProjectData::hasHugeCgns() const
{
	QList<CgnsFileEntry*> cgnsFiles = m_mainfile->cgnsFileList()->cgnsFiles();
	for (int i = 0; i < cgnsFiles.count(); ++i) {
		CgnsFileEntry* entry = cgnsFiles.at(i);
		QString filename = workCgnsFileName(entry->filename());
		QFileInfo finfo(filename);
		if (finfo.size() > 2000000000) {
			return true;
		}
	}
	return false;
}

void ProjectData::openPostProcessors()
{
	// load post-processor settings
	mainfile()->openPostProcessors();
}

bool ProjectData::isInWorkspace() const
{
	if (m_mainWindow == nullptr) {return false;}
	QString wsPath = m_mainWindow->workspace()->workspace().absolutePath();
	return m_workDirectory.contains(wsPath);
}
