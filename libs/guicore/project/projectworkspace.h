#ifndef PROJECTWORKSPACE_H
#define PROJECTWORKSPACE_H

#include "../guicore_global.h"

#include <QObject>
#include <QString>
#include <QList>
#include <QDir>

#include <vector>

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
	/// Folder name of workspace.
	const static QString FOLDERNAME;

	explicit ProjectWorkspace(QWidget* parent = nullptr);
	~ProjectWorkspace();

	std::vector<WorkfolderInfo*> workfolders() const;

	/// Return the list of workfolders under this workspace,
	/// those seems to be not used any more.
	std::vector<WorkfolderInfo*> trashWorkfolders();

	/// Trash all workfolders those seems to be not used any more.
	void trashAllTrashWorkfolders();
	/// Trash the specified workfolder.
	bool deleteWorkfolder(const QString& foldername);

	/// QDir object that points the workspace folder.
	const QDir& workspace() const;
	QString tmpFileName() const;
	/// Remove files created by zip.exe as temporary files.
	void removeZipTrashes();

public slots:
	void updateWorkfolderList();

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

#ifdef _DEBUG
	#include "private/projectworkspace_impl.h"
#endif // _DEBUG

#endif // PROJECTWORKSPACE_H
