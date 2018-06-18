#include "projectworkspace.h"
#include "private/projectworkspace_impl.h"
#include "workfolderinfo.h"

#include <misc/filesystemfunction.h>

#include <QCryptographicHash>
#include <QDir>
#include <QMessageBox>
#include <QSettings>
#include <QTime>

const QString ProjectWorkspace::FOLDERNAME = ".iRIC_workspace";

ProjectWorkspace::Impl::Impl(ProjectWorkspace* parent) :
	m_parent {parent}
{
	QDir homeDir = QDir::home();
	QString defaultWorkspace = homeDir.absoluteFilePath(ProjectWorkspace::FOLDERNAME);
	QSettings settings;
	QString workspace = settings.value("general/workspace", defaultWorkspace).value<QString>();
	m_workspace = QDir(workspace);

	if (! m_workspace.exists()) {
		QDir parentDir = m_workspace;
		parentDir.cdUp();
		parentDir.mkdir(m_workspace.dirName());
	}

	updateWorkfolderList();
}

void ProjectWorkspace::Impl::updateWorkfolderList()
{
	for (auto f : m_workfolderList) {
		delete f;
	}
	m_workfolderList.clear();
	QStringList subdirs = m_workspace.entryList(QStringList(), QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);

	for (int i = 0; i < subdirs.size(); ++i) {
		QString wfFull = m_workspace.absoluteFilePath(subdirs.at(i));
		WorkfolderInfo* info = new WorkfolderInfo(wfFull, m_parent);
		m_workfolderList.push_back(info);
	}
}

ProjectWorkspace::ProjectWorkspace(QWidget* parent) :
	QObject {parent},
	impl {new Impl(this)}
{
	int trashCount = static_cast<int>(trashWorkfolders().size());
	if (trashCount == 0) {return;}

	QString workPath = QDir::toNativeSeparators(impl->m_workspace.absolutePath());
	QString msg = ProjectWorkspace::tr("Workspace \"%1\" has %2 project folders that seems to be not used any more. Do you want to remove them?").arg(workPath).arg(trashCount);

	int ret = QMessageBox::warning(parent, ProjectWorkspace::tr("Information"), msg, QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if (ret == QMessageBox::No) {return;}

	trashAllTrashWorkfolders();
}

ProjectWorkspace::~ProjectWorkspace()
{
	delete impl;
}

std::vector<WorkfolderInfo*> ProjectWorkspace::workfolders() const
{
	return impl->m_workfolderList;
}

std::vector<WorkfolderInfo*> ProjectWorkspace::trashWorkfolders()
{
	std::vector<WorkfolderInfo*> ret;
	for (WorkfolderInfo* f : impl->m_workfolderList) {
		if (! f->isLocked()) {
			ret.push_back(f);
		}
	}
	return ret;
}

void ProjectWorkspace::trashAllTrashWorkfolders()
{
	auto trashes = trashWorkfolders();
	for (WorkfolderInfo* f : trashes) {
		deleteWorkfolder(f->folderName());
	}
}

bool ProjectWorkspace::deleteWorkfolder(const QString& foldername)
{
	return iRIC::rmdirRecursively(impl->m_workspace.absoluteFilePath(foldername));
}

const QDir& ProjectWorkspace::workspace() const
{
	return impl->m_workspace;
}

QString ProjectWorkspace::tmpFileName() const
{
	QCryptographicHash hash(QCryptographicHash::Md5);
	QTime current = QTime::currentTime();
	qsrand(current.msec());
	hash.addData(QByteArray(1, qrand()));
	QDir workDir(impl->m_workspace.absolutePath());

	QString filename = hash.result().toHex();
	while (workDir.exists(filename)) {
		hash.addData(QByteArray(1, qrand()));
		filename = hash.result().toHex();
	}
	return workDir.absoluteFilePath(filename);
}

void ProjectWorkspace::removeZipTrashes()
{
	QStringList nameFilters;
	nameFilters << "zia*";
	QStringList trashFiles = impl->m_workspace.entryList(nameFilters, QDir::Files);
	for (int i = 0; i < trashFiles.count(); ++i) {
		QString trashFile = trashFiles.at(i);
		impl->m_workspace.remove(trashFile);
	}
}

void ProjectWorkspace::updateWorkfolderList()
{
	impl->updateWorkfolderList();
}
