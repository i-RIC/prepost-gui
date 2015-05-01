#ifndef PROJECTWORKSPACE_H
#define PROJECTWORKSPACE_H

#include "../guicore_global.h"

#include <QObject>
#include <QString>
#include <QList>
#include <QDir>

class WorkfolderInfo;

/// This class handles the workspace of iRIC for the current user.
/**
 * iRIC uses iRIC_workspace folder under user document folder
 * as iRIC workspace. All work directories of iRIC is created
 * under this folder.
 */
class GUICOREDLL_EXPORT ProjectWorkspace : public QObject
{
	Q_OBJECT
public:
	/// Foldeer name of workspace.
	const static QString FOLDERNAME;
	/// Constructor
	explicit ProjectWorkspace(QObject* parent = nullptr);
	/// The list of workfolders in the current workspace.
	const QList<WorkfolderInfo*> workfolders() const {return m_workfolderList;}
	/// Return the list of workfolders under this workspace,
	/// those seems to be not used any more.
	QList<WorkfolderInfo*> trashWorkfolders();
	/// Trash all workfolders those seems to be not used any more.
	void trashAllTrashWorkfolders();
	/// Trash the specified workfolder.
	/**
	 * @return bool True if succeeded, and false if failed.
	 */
	bool trashWorkfolder(const QString& foldername);
	/// QDir object that points the workspace folder.
	const QDir& workspace() const {return m_workspace;}
	const QString tmpFileName() const;
	/// Remove files created by zip.exe as temporary files.
	void removeZipTrashes();

public slots:
	/// Update workfolder list
	void updateWorkfolderList();

private:
	QList<WorkfolderInfo*> m_workfolderList;
	QDir m_workspace;
};

#endif // PROJECTWORKSPACE_H
