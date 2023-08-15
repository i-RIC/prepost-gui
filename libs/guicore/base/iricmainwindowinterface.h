#ifndef IRICMAINWINDOWINTERFACE_H
#define IRICMAINWINDOWINTERFACE_H

#include "../guicore_global.h"
#include <QMainWindow>

class AnimationControllerInterface;
class CoordinateSystemBuilder;
class ExecuterErrorHandler;
class ExecuterI;
class ExecuterWatcher;
class PreProcessorWindowInterface;
class ProjectData;
class ProjectWorkspace;
class VersionNumber;

class QDomNode;
class QLocale;
class QProcessEnvironment;
class QXmlStreamWriter;

class GUICOREDLL_EXPORT iRICMainWindowInterface : public QMainWindow
{

public:
	static const int STATUSBAR_DISPLAYTIME = 5000;

	iRICMainWindowInterface(QWidget* widget);
	virtual ~iRICMainWindowInterface() {}

	virtual PreProcessorWindowInterface* preProcessorWindow() const = 0;
	virtual void setContinuousSnapshotInProgress(bool prog) = 0;
	virtual bool continuousSnapshotInProgress() const = 0;
	virtual bool isSolverRunning() const = 0;
	virtual void warnSolverRunning() const = 0;
	virtual AnimationControllerInterface* animationController() const = 0;
	virtual void initForSolverDefinition() = 0;
	virtual CoordinateSystemBuilder* coordinateSystemBuilder() const = 0;
	virtual void loadSubWindowsFromProjectMainFile(const QDomNode& node) = 0;
	virtual void saveSubWindowsToProjectMainFile(QXmlStreamWriter& writer) = 0;
	virtual int loadFromCgnsFile() = 0;
	virtual int saveToCgnsFile() = 0;
	virtual int updateCgnsFileOtherThanGrids() = 0;
	virtual void closeCgnsFile() = 0;
	virtual void toggleGridEditFlag() = 0;
	virtual QStringList containedFiles() const = 0;
	virtual ProjectWorkspace* workspace() = 0;
	virtual const VersionNumber& versionNumber() const = 0;

	virtual ProjectData* projectData() const = 0;
	virtual void setProjectData(ProjectData* projectData) = 0;

	virtual const QLocale locale() const = 0;
	virtual const QProcessEnvironment& processEnvironment() const = 0;
	virtual void updateCrosssectionWindows() = 0;

	virtual ExecuterWatcher* buildExecuteWatcher(ExecuterI* executer) = 0;
	virtual bool cuiMode() const = 0;

public slots:
	virtual bool saveProject() = 0;

	virtual void enterModelessDialogMode() = 0;
	virtual void exitModelessDialogMode() = 0;
	virtual void updateWindowList() = 0;
	virtual void importMeasuredData() = 0;
};

#endif // IRICMAINWINDOWINTERFACE_H
