#include "projectdata.h"
#include "projectmainfile.h"
#include "cgnsfilelist.h"
#include <misc/filesystemfunction.h>
#include "../base/iricmainwindowinterface.h"
#include <misc/ziparchive.h>
#include <guibase/waitdialog.h>
#include "../pre/base/preprocessorwindowinterface.h"
//#include "projectpropertydialog.h"
//#include "solverconsole/solverconsolewindow.h"
//#include "main/iricmainwindowactionmanager.h"
//#include "project/projectpropertydialog.h"
#include "../solverdef/solverdefinitiongridtype.h"
#include "../solverdef/solverdefinitiongridrelatedcondition.h"
//#include "commongui/waitdialog.h"
#include "projectworkspace.h"
#include "../postcontainer/postsolutioninfo.h"

#include <QDir>
#include <QFile>
#include <QTime>
#include <QCryptographicHash>
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>
#include <QCoreApplication>
#include <QThread>
#include <QProcess>

class ProjectDataMoveThread : public QThread {
public:
	ProjectDataMoveThread(const QString& from, const QString& to, QObject* parent)
		: QThread(parent)
	{
		m_from = from;
		m_to = to;
	}
	bool result(){return m_result;}
protected:
	void run(){
		m_result = iRIC::moveDirContent(m_from, m_to);
	}
private:
	QString m_from;
	QString m_to;
	bool m_result;
};

class ProjectDataCopyThread : public QThread {
public:
	ProjectDataCopyThread(const QString& from, const QString& to, QObject* parent)
		: QThread(parent)
	{
		m_from = from;
		m_to = to;
	}
	bool result(){return m_result;}
protected:
	void run(){
		m_result = iRIC::copyDirRecursively(m_from, m_to);
	}
private:
	QString m_from;
	QString m_to;
	bool m_result;
};

const QString ProjectData::LOCKFILENAME = "lock";

ProjectData::ProjectData(const QString& workdir, iRICMainWindowInterface *parent) :
	QObject(parent)
{
	m_mainWindow = parent;
	m_workDirectory = workdir;
	m_solverDefinition = 0;
	m_folderProject = false;
	// if the workdirectory doesn't exists, make it.
	QDir wdir(m_workDirectory);
	if (! wdir.exists()){
		QDir parent = wdir;
		parent.cdUp();
		parent.mkdir(wdir.dirName());
	}
	// create ProjectMainFile.
	m_mainfile = new ProjectMainFile(this);
	m_lockFile = 0;
	lock();
	connect(m_mainfile, SIGNAL(cgnsFileSwitched()), parent, SLOT(handleCgnsSwitch()));

	if (parent == 0){
		// this project data should not attatched to mainwindow.
		return;
	}
	parent->setProjectData(this);
}

ProjectData::~ProjectData()
{
	if (m_solverDefinition != 0){
		delete m_solverDefinition;
	}
	// delete mainfile
	delete m_mainfile;
	// close lock file
	unlock();

	// remove the workdirectory recursively, if it is under workspace.
	if (isInWorkspace()){
		iRIC::rmdirRecursively(m_workDirectory);
	}
}

const QString ProjectData::absoluteFileName(const QString& relativeFileName) const
{
	QDir workdir(m_workDirectory);
	return workdir.absoluteFilePath(relativeFileName);
}

bool ProjectData::unzipFrom(const QString &filename)
{
	// Unzip archive file
	QProcess* process = iRIC::createUnzipArchiveProcess(filename, workDirectory());
	m_finished = false;
	connect(process, SIGNAL(finished(int)), this, SLOT(handleFinish()));
	// wait for 3 second first.
	m_finished = process->waitForFinished(3000);
	qApp->processEvents();
	iRICMainWindowInterface* mw = mainWindow();
	if (mw != 0){mw->enterModelessDialogMode();}
	if (! m_finished){
		int prog = 10;
		// show dialog and wait.
		m_canceled = false;
		m_waitDialog = new WaitDialog(m_mainWindow);
		m_waitDialog->showProgressBar();
		m_waitDialog->setRange(0, 100);
		m_waitDialog->setUnknownLimitMode(300);
		m_waitDialog->setProgress(prog);
		m_waitDialog->setMessage(tr("Loading project file..."));

		m_waitDialog->show();
		connect(m_waitDialog, SIGNAL(canceled()), this, SLOT(cancel()));
		while (! m_finished && ! m_canceled){
			qApp->processEvents();
			process->waitForFinished(200);
			m_waitDialog->setProgress(prog);
			++prog;
		}
		m_waitDialog->hide();
		delete m_waitDialog;
		m_waitDialog = 0;
		if (m_canceled){
			// not finished, but canceled.
			process->kill();
			delete process;
			mainWindow()->exitModelessDialogMode();
			return false;
		}
	}
	delete process;
	if (mw != 0){mw->exitModelessDialogMode();}
	// save the filename.
	m_filename = filename;
	return true;
}

void ProjectData::loadSolverInformation()
{
	// load project.xml. (only solver name and version)
	mainfile()->loadSolverInformation();
}

void ProjectData::setSolverDefinition(SolverDefinition* def){
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

bool ProjectData::zipTo(const QString &filename)
{
	QStringList filelist;
	QDir workdir(workDirectory());
	filelist << workdir.relativeFilePath(mainfile()->filename());
	filelist << mainfile()->containedFiles();

	// define temporary file used as output file of zip process.
	QString outfile = m_mainWindow->workspace()->tmpFileName();
	outfile.append(".zip");

	// zip archive file
	QProcess* process = iRIC::createZipArchiveProcess(outfile, workDirectory(), filelist);
	m_finished = false;
	connect(process, SIGNAL(finished(int)), this, SLOT(handleFinish()));

	// wait for 3 second first.
	int waitTime = 3000;
	m_finished = process->waitForFinished(waitTime);
	qApp->processEvents();
	if (! m_finished){
		int prog = 10;
		// show dialog and wait.
		m_canceled = false;
		m_waitDialog = new WaitDialog(m_mainWindow);
		m_waitDialog->showProgressBar();
		m_waitDialog->setRange(0, 100);
		m_waitDialog->setUnknownLimitMode(300);
		m_waitDialog->setProgress(prog);
		m_waitDialog->setMessage(tr("Saving project file..."));

		m_waitDialog->show();
		connect(m_waitDialog, SIGNAL(canceled()), this, SLOT(cancel()));
		while (! m_finished && ! m_canceled){
			qApp->processEvents();
			process->waitForFinished(200);
			m_waitDialog->setProgress(prog);
			++ prog;
		}
		m_waitDialog->setFinished();
		m_waitDialog->hide();
		delete m_waitDialog;
		m_waitDialog = 0;
		if (m_canceled){
			// not finished, but canceled.
			process->kill();
			delete process;

			// remove the temporary file that zip process used. it is created
			// in the workspace.
			mainWindow()->workspace()->removeZipTrashes();
			return false;
		}
	}
	delete process;
	if (QFile::exists(filename)){
		bool ok = QFile::remove(filename);
		if (! ok){
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

const QString ProjectData::newWorkfolderName(const QDir& workspace)
{
	QCryptographicHash hash(QCryptographicHash::Md5);
	QTime current = QTime::currentTime();
	qsrand(current.msec());
	hash.addData(QByteArray(1, qrand()));

	QString dirname = hash.result().toHex();
	while (workspace.exists(dirname)){
		hash.addData(QByteArray(1, qrand()));
		dirname = hash.result().toHex();
	}
	return workspace.absoluteFilePath(dirname);
}
bool ProjectData::switchToDefaultCgnsFile()
{
	QString current = m_mainfile->cgnsFileList()->current()->filename();
	return m_mainfile->switchCgnsFile(current);
}

const QString ProjectData::workCgnsFileName(const QString& name)
{
	QString tmpstr = name;
	return QDir(m_workDirectory).absoluteFilePath(tmpstr.append(".cgn"));
}

const QString ProjectData::currentCgnsFileName()
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

const QString ProjectData::tmpFileName()
{
	QCryptographicHash hash(QCryptographicHash::Md5);
	QTime current = QTime::currentTime();
	qsrand(current.msec());
	hash.addData(QByteArray(1, qrand()));
	QDir workDir(m_workDirectory);

	QString filename = hash.result().toHex();
	while (workDir.exists(filename)){
		hash.addData(QByteArray(1, qrand()));
		filename = hash.result().toHex();
	}
	return workDir.absoluteFilePath(filename);
}

/*
void ProjectData::showPropertyDialog()
{
	ProjectPropertyDialog dialog(mainWindow());
	dialog.setProjectData(this);
	dialog.exec();
}
*/

void ProjectData::checkGridConditions()
{
	QList<SolverDefinitionGridType*>::const_iterator it;
	QStringList ngtypes;
	const QList<SolverDefinitionGridType*> gtypes = m_solverDefinition->gridTypes();
	for (it = gtypes.begin(); it != gtypes.end(); ++it){
		const SolverDefinitionGridType* gt = *it;
		bool ok = false;
		ok = (gt->defaultGridType() != SolverDefinitionGridType::gtStructured2DGrid);
		const QList<SolverDefinitionGridRelatedCondition*>& conds = gt->gridRelatedConditions();
		QList<SolverDefinitionGridRelatedCondition*>::const_iterator cit;
		for (cit = conds.begin(); cit != conds.end(); ++cit){
			const SolverDefinitionGridRelatedCondition* cond = *cit;
			ok = ok || (cond->name() == "Elevation");
		}
		if (! ok){
			ngtypes.append(gt->caption());
		}
	}
	if (! ngtypes.empty()){
		// NG grid types were found.
		QMessageBox::warning(
			m_mainWindow, tr("Warning"),
			tr("Elevation grid attribute is not defined for grid type %1. Grid I/O functions, grid creating functions may not work correnctly for these grid types.").arg(ngtypes.join(", ")));
	}
}

bool ProjectData::lock()
{
	unlock();
	QString lockFilename = QDir(m_workDirectory).absoluteFilePath(ProjectData::LOCKFILENAME);
	m_lockFile = new QFile(lockFilename, this);
	bool ok = m_lockFile->open(QIODevice::WriteOnly);
	if (! ok){
		delete m_lockFile;
		m_lockFile = 0;
		return false;
	}
	return true;
}

void ProjectData::unlock()
{
	if (m_lockFile == 0){return;}
	m_lockFile->close();
	delete m_lockFile;
	QString lockFilename = QDir(m_workDirectory).absoluteFilePath(ProjectData::LOCKFILENAME);
	QFile::remove(lockFilename);
	m_lockFile = 0;
}

bool ProjectData::moveTo(const QString& newWorkFolder)
{
	if (QFile::exists(newWorkFolder)){
		if (! iRIC::rmdirRecursively(newWorkFolder)){
			return false;
		}
		qApp->processEvents();
		QFileInfo finfo(newWorkFolder);
		QDir dir(finfo.absolutePath());
		if (! dir.mkdir(finfo.fileName())){
			return false;
		}
		qApp->processEvents();
	}
	#ifdef Q_OS_WIN32
		QString oldDrive = m_workDirectory.left(1).toUpper();
		QString newDrive = newWorkFolder.left(1).toUpper();
		if (oldDrive != newDrive){
			// moving is done between different drives.
			// execute copy and remove.
			QString currentWorkDirectory = m_workDirectory;
			bool ok = copyTo(newWorkFolder, true);
			if (! ok){return false;}
			ok = iRIC::rmdirRecursively(currentWorkDirectory);
			// return true, even if rmdir failed.
			return true;
		}
	#endif // Q_OS_WIN32

/*
	if (QFile::exists(newWorkFolder))
	{
		// The specified folder or file already exists. Try to remove it first.
		bool ok = iRIC::rmdirRecursively(newWorkFolder);
		// removing the existing folder failed.
		if (! ok){return false;}
	}
*/
	// move current folder to new folder.
	unlock();
	mainfile()->postSolutionInfo()->close();

	ProjectDataMoveThread* thread = new ProjectDataMoveThread(m_workDirectory, newWorkFolder, this);
	m_finished = false;
	connect(thread, SIGNAL(finished()), this, SLOT(handleFinish()));
	thread->start();
	/// wait for 3 seconds first.
	int waitTime = 3000;
	m_finished = thread->wait(waitTime);
	qApp->processEvents();
	if (! m_finished){
		int prog = 10;
		// show dialog and wait.
		m_canceled = false;
		m_waitDialog = new WaitDialog(m_mainWindow);
		m_waitDialog->showProgressBar();
		m_waitDialog->setRange(0, 100);
		m_waitDialog->setUnknownLimitMode(300);
		m_waitDialog->setProgress(prog);
		m_waitDialog->setMessage(tr("Saving project..."));

		m_waitDialog->show();
		connect(m_waitDialog, SIGNAL(canceled()), this, SLOT(cancel()));
		while (! m_finished && ! m_canceled){
			qApp->processEvents();
			thread->wait(200);
			m_waitDialog->setProgress(prog);
			++prog;
		}
		m_waitDialog->setFinished();
		m_waitDialog->hide();
		delete m_waitDialog;
		m_waitDialog = 0;
		if (m_canceled){
			// not finished, but canceled.
			thread->terminate();
			delete thread;
			lock();
			return false;
		}
	}
	bool ok = thread->result();
	if (! ok){
		// moving failed. keep the current workspace.
		lock();
		return false;
	}
	// moving succeeded.
	m_workDirectory = newWorkFolder;
	ok = lock();
	return ok;
}

bool ProjectData::copyTo(const QString& newWorkFolder, bool switchToNewFolder)
{
	if (QFile::exists(newWorkFolder)){
		QFileInfo finfo(newWorkFolder);
		QString parentPath = finfo.absolutePath();
		QDir dir(parentPath);
		QString dirName = finfo.fileName();
		if (! iRIC::rmdirRecursively(newWorkFolder)){
			return false;
		}
		qApp->processEvents();
		if (! dir.mkdir(dirName)){
			return false;
		}
		qApp->processEvents();
	}
	// copy current folder to new folder.
	unlock();
	mainfile()->postSolutionInfo()->close();

	ProjectDataCopyThread* thread = new ProjectDataCopyThread(m_workDirectory, newWorkFolder, this);
	m_finished = false;
	connect(thread, SIGNAL(finished()), this, SLOT(handleFinish()));
	thread->start();
	/// wait for 3 seconds first.
	int waitTime = 3000;
	m_finished = thread->wait(waitTime);
	qApp->processEvents();
	if (! m_finished){
		int prog = 10;
		// show dialog and wait.
		m_canceled = false;
		m_waitDialog = new WaitDialog(m_mainWindow);
		m_waitDialog->showProgressBar();
		m_waitDialog->setRange(0, 100);
		m_waitDialog->setUnknownLimitMode(300);
		m_waitDialog->setProgress(prog);
		m_waitDialog->setMessage(tr("Saving project..."));

		m_waitDialog->show();
		connect(m_waitDialog, SIGNAL(canceled()), this, SLOT(cancel()));
		while (! m_finished && ! m_canceled){
			qApp->processEvents();
			thread->wait(200);
			m_waitDialog->setProgress(prog);
			++prog;
		}
		m_waitDialog->setFinished();
		m_waitDialog->hide();
		delete m_waitDialog;
		m_waitDialog = 0;
		if (m_canceled){
			// not finished, but canceled.
			thread->terminate();
			thread->wait();
			delete thread;

			goto ERROR;
		}
	}
	bool ok = thread->result();
	if (! ok){
		goto ERROR;
	}

	// copying succeeded.
	if (switchToNewFolder){
		m_workDirectory = newWorkFolder;
	}

	ok = lock();
	return ok;

ERROR:
	iRIC::rmdirRecursively(newWorkFolder);
	lock();
	return false;
}

bool ProjectData::hasHugeCgns()
{
	QList<CgnsFileList::CgnsFileEntry*> cgnsFiles = m_mainfile->cgnsFileList()->cgnsFiles();
	for (int i = 0; i < cgnsFiles.count(); ++i){
		CgnsFileList::CgnsFileEntry* entry = cgnsFiles.at(i);
		QString filename = workCgnsFileName(entry->filename());
		QFileInfo finfo(filename);
		if (finfo.size() > 2000000000){
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

bool ProjectData::isInWorkspace()
{
	if (m_mainWindow == 0){return false;}
	QString wsPath = m_mainWindow->workspace()->workspace().absolutePath();
	return m_workDirectory.contains(wsPath);
}
