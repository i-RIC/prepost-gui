#ifndef PROJECTDATA_H
#define PROJECTDATA_H

#include "../guicore_global.h"

#include <QObject>
#include <QString>

class QDir;
class QFile;
class iRICMainWindowInterface;
class ProjectMainFile;
class SolverDefinition;
class VersionNumber;
class WaitDialog;

/// ProjectData class represents the container that stores all information related
/// to the current project.
/**
 * ProjectData contains the data refered in @ref projectdata . It handles loading
 * and saving processes fron/into zip files.
 *
 * You can create a new project data using the constructor ProjectData::ProjectData().
 * You can load data from project file by calling ProjectData::load() , and save
 * into project file by calling ProjectData::save().
 *
 *
 */
class GUICOREDLL_EXPORT ProjectData : public QObject
{
	Q_OBJECT

public:
	/// File name of lock file (that is put just under the workfolder)
	const static QString LOCKFILENAME;
	/// Constructor
	explicit ProjectData(const QString& workdir, iRICMainWindowInterface* parent);
	/// Destructor
	virtual ~ProjectData();
	/// Returns the absolute file path of a file included in the project data.
	const QString absoluteFileName(const QString& relativeFileName) const;
	/// Work directory
	const QString& workDirectory() {return m_workDirectory;}
	/// File name to save (or loaded from)
	const QString& filename() {return m_filename;}
	/// Set file name to save
	void setFilename(const QString& fname, bool isFolder) {
		m_filename = fname;
		m_folderProject = isFolder;
	}
	/// Unzip the specified file into workfolder.
	bool unzipFrom(const QString& zipFilename);
	/// Load solver information from a project file.
	void loadSolverInformation();
	/// Load project data from a project file.
	void load();
	/// Load only CGNS file list from a project file.
	void loadCgnsList();
	/// Compress the workfolder content to specified zip file..
	bool zipTo(const QString& filename);
	/// Save project data into the work folder.
	bool save();
	/// Main file (project.xml)
	ProjectMainFile* mainfile() const {return m_mainfile;}
	/// Solver definition
	SolverDefinition* solverDefinition() const {return m_solverDefinition;}
	/// Setter function for solver definition
	void setSolverDefinition(SolverDefinition* def);
	/// Switch to the default cgns file.
	bool switchToDefaultCgnsFile();
	/// Set iRIC version number
	void setVersion(const VersionNumber& v);
	/// Get iRIC version number that created this project.
	const VersionNumber version();
	/// Get new workfolder name to create
	static const QString newWorkfolderName(const QDir& workspace);
	const QString workCgnsFileName(const QString& name) const;
	const QString currentCgnsFileName() const;
	/// iRIC Main Window.
	iRICMainWindowInterface* mainWindow() const {return m_mainWindow;}
	const QString tmpFileName() const;
	/// Move the work folder to the specified folder. Old folder is removed.
	bool moveTo(const QString& newWorkFolder);
	/// Copy the work folder to the specified folder. Old folder also is kept.
	bool copyTo(const QString& newWorkFolder, bool switchToNewFolder);
	/// Whether it is a folder project.
	bool folderProject() const {return m_folderProject;}
	/// Returns true when CGNS file size exceeds 2 GB.
	bool hasHugeCgns() const;
	/// Open the post-processors using the settings in project file.
	void openPostProcessors();
	/// Project data work directory is inside workspace.
	bool isInWorkspace() const;

public slots:
	/// Open work directory with Explorer.
	void openWorkDirectory();

private slots:
	/// Cancel open / save operation.
	void cancel() {m_canceled = true;}
	/// Event handler for open / save operation finish signal.
	void handleFinish() {m_finished = true;}

private:
	/// Open lock file as write mode, so that other process can know that this project
	/// folder is active.
	bool lock();
	/// Close lock file.
	void unlock();
	void checkGridConditions();
	/// Initializes project data for the specified solver definition.
	void initForSolverDefinition();
	/// Work directory, like "~/.iRIC_workspace/project1"
	QString m_workDirectory;
	/// File name to save (or loaded from).
	QString m_filename;
	/// This project is saved as folder.
	bool m_folderProject;
	/// Main file object, that handles information written in "project.xml".
	ProjectMainFile* m_mainfile;
	/// Solver Definition container.
	SolverDefinition* m_solverDefinition;
	/// Lock file
	QFile* m_lockFile;
	/// iRIC MainWindow
	iRICMainWindowInterface* m_mainWindow;
	/// When opening or saving is finished, it's true.
	bool m_finished;
	/// When opening or saving is canceled, it's true.
	bool m_canceled;
	/// Wait dialog shown when opening or saving project.
	WaitDialog* m_waitDialog;
};

#endif // PROJECTDATA_H
