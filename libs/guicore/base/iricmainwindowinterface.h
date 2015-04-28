#ifndef IRICMAINWINDOWINTERFACE_H
#define IRICMAINWINDOWINTERFACE_H

#include "../guicore_global.h"
#include <QMainWindow>

class QLocale;
class QDomNode;
class QXmlStreamWriter;
class ProjectWorkspace;
class PreProcessorWindowInterface;
class AnimationControllerInterface;
class CoordinateSystemBuilder;
class VersionNumber;
class ProjectData;
class QProcessEnvironment;

class GUICOREDLL_EXPORT iRICMainWindowInterface : public QMainWindow {
public:
	static const int STATUSBAR_DISPLAYTIME = 5000;
	/// constructor
	iRICMainWindowInterface(QWidget* widget);
	/// destructor
	virtual ~iRICMainWindowInterface(){}

	virtual PreProcessorWindowInterface* preProcessorWindow() = 0;
	virtual void setContinuousSnapshotInProgress(bool prog) = 0;
	virtual bool continuousSnapshotInProgress() = 0;
	virtual bool isSolverRunning() = 0;
	virtual void warnSolverRunning() = 0;
	virtual AnimationControllerInterface* animationController() = 0;
	virtual void initForSolverDefinition() = 0;
	virtual CoordinateSystemBuilder* coordinateSystemBuilder() const = 0;
	virtual void loadSubWindowsFromProjectMainFile(const QDomNode& node) = 0;
	virtual void saveSubWindowsToProjectMainFile(QXmlStreamWriter& writer) = 0;
	virtual void loadFromCgnsFile(const int fn) = 0;
	virtual void saveToCgnsFile(const int fn) = 0;
	virtual void closeCgnsFile() = 0;
	virtual void toggleGridEditFlag() = 0;
	virtual void clearResults() = 0;
	virtual QStringList containedFiles() const = 0;
	virtual ProjectWorkspace* workspace() = 0;
	virtual const VersionNumber versionNumber() const = 0;
	virtual bool clearResultsIfGridIsEdited() = 0;
	virtual void setProjectData(ProjectData* projectData) = 0;
	virtual const QLocale locale() = 0;
	virtual const QProcessEnvironment& processEnvironment() const = 0;

public slots:
	virtual bool saveProject() = 0;

	virtual void enterModelessDialogMode() = 0;
	virtual void exitModelessDialogMode() = 0;

};

#endif // IRICMAINWINDOWINTERFACE_H
