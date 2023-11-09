#ifndef IRICMAINWINDOW_H
#define IRICMAINWINDOW_H

#include "../gui_api.h"

#include "../continuoussnapshot/continuoussnapshotsetting.h"

#include <guicore/base/iricmainwindowi.h>
#include <misc/versionnumber.h>
#include <pre/preprocessorwindow.h>

#include <QMainWindow>
#include <QLocale>
#include <QList>
#include <QProcessEnvironment>

namespace iRICPython {
	class Application;
} // iRICPython


class QMenu;
class QToolbar;
class QAction;
class QMdiArea;
class QMdiSubWindow;
class QXmlStreamWriter;
class QDomNode;
class iRICMetaData;

class AnimationController;
class ContinuousSnapshotWizard;
class CoordinateSystemDisplayWidget;
class SolverConsoleWindow;
class PostProcessorWindow;
class PreProcessorWindow;
class SolverDefinitionList;
class SolverDefinition;
class SolverDefinitionAbstract;
class ProjectWorkspace;
class MousePositionWidget;
class QMainWindowWithSnapshotResizeWidget;
class Vtk2dGraphicsViewAngleWidget;
class VtkGraphicsViewScaleWidget;
class VTK2DGraphicsView;
class GridCreatingConditionCreator;
class CoordinateSystemBuilder;

class ProjectData;
class iRICMainWindowActionManager;
class iRICMainWindowMiscDialogManager;
class PostProcessorWindowFactory;

/// This class represents the main window of iRIC 2.0.
class GUI_API iRICMainWindow : public iRICMainWindowI
{
	Q_OBJECT

public:
	iRICMainWindow(bool cuiMode = false, QWidget* parent = nullptr);
	~iRICMainWindow();

	/// The PreProcessorWindow
	PreProcessorWindow* preProcessorWindow() const override;
	/// The SolverConsoleWindow
	SolverConsoleWindow* solverConsoleWindow() const;

	const QLocale locale() const override;
	bool isSolverRunning() const override;
	void warnSolverRunning() const override;
	ProjectWorkspace* workspace() override;
	const VersionNumber& versionNumber() const override;
	void setDebugMode(bool debug);
	bool isDebugMode() const;
	bool continuousSnapshotInProgress() const override;
	void setContinuousSnapshotInProgress(bool prog) override;
	QString tmpFileName(int len = 0) const;
	AnimationControllerI* animationController() const override;
	CoordinateSystemBuilder* coordinateSystemBuilder() const override;
	void initForSolverDefinition() override;

	void loadSubWindowsFromProjectMainFile(const QDomNode& node) override;
	void saveSubWindowsToProjectMainFile(QXmlStreamWriter& writer) override;

	QStringList containedFiles() const override;
	int loadFromCgnsFile() override;
	int saveToCgnsFile() override;
	int updateCgnsFileOtherThanGrids() override;
	void closeCgnsFile() override;
	void toggleGridEditFlag() override;

	ProjectData* projectData() const override;
	void setProjectData(ProjectData* projectData) override;

	/// Check whether work folder is set to a good folder. If it is good returns true.
	bool checkWorkFolderWorks();
	void importCalculationResult(const QString& filename);
	const QProcessEnvironment& processEnvironment() const override;
	void updateCrosssectionWindows() override;

	ExecuterWatcher* buildExecuteWatcher(ExecuterI* executer) override;
	bool cuiMode() const override;

private:
	void parseArgs();
	/// Save Window State (normal, maximized, or minimized, windowsize)
	void saveWindowState();
	/// Restore Window State (normal, maximized, or minimized, windowsize)
	void restoreWindowState();
	/// Load plugin dlls.
	void loadPlugins();
	/// Initialize settings.
	void initSetting();
	/// The central widget
	QMdiArea* m_centralWidget;
	// pointers to inner windows
	PreProcessorWindow* m_preProcessorWindow;
	SolverConsoleWindow* m_solverConsoleWindow;

protected:
	void closeEvent(QCloseEvent*) override;

public slots:
	void openStartDialog();
	/// open dialog and create new project.
	void newProject();
	/// Create new project for the specified solver.
	/**
	 * This method simply call newProject(SolverDefinitionAbstract*) .
	 * This method is defined to make it possible to recieve signal
	 * QSignalMap::map(QObject*) .
	 */
	void newProject(QObject* solver);
	/// Create new project for the specified solver.
	void newProject(SolverDefinitionAbstract* solver);
	/// Open a project file
	void openProject();
	/// Open a project file
	void openProject(const QString& filename);
	/// Open a recently opened (or saved) project.
	void openRecentProject();
	/// Reload cgns file
	void reloadCgnsFile();
	/// Close the current project.
	bool closeProject();
	/// Save current project
	bool saveProject() override;
	/// Save current project
	bool saveProject(const QString& name, bool asFolder);
	/// Save current project. Show a dialog to select filename.
	bool saveProjectAsFile();
	/// Save current project. Show a dialog to select foldername.
	bool saveProjectAsFolder();
	/// Export current cgns file.
	void exportCurrentCgnsFile();
	/// Export Calculation result into VTK files.
	void exportCalculationResult();
	/// Export Particles into VTK files.
	void exportParticles();
	/// Export Coutour figure as ESRI Shapefiles.
	void exportCfShape();
	/// Export Google Earth KMZ for street view
	void exportStKMZ();
	/// Import Visualization/Graph settings.
	void importVisGraphSetting();
	/// Export Visualization/Graph settings.
	void exportVisGraphSetting();

	void checkCgnsStepsUpdate();
	void ActiveSubwindowChanged(QMdiSubWindow* newActiveWindow);
	/// Do the tasks to do when the CGNS file is switched.
	void handleCgnsSwitch();
	void focusPreProcessorWindow();
	void focusSolverConsoleWindow();

	void create2dPostWindow();
	void create2dBirdEyePostWindow();
	void create3dPostWindow();
	void createGraph2dHybridWindow();
	void createGraph2dScatteredWindow();
	void openVerificationDialog();

	void setCurrentStep(unsigned int newstep);
	/// Enter modeless dialog mode. menus, toolbars are disabled.
	void enterModelessDialogMode() override;
	/// Exit modeless dialog mode. Menus, toolbars becomes usable.
	void exitModelessDialogMode() override;
	void updateWindowList() override;

	void setupRecentProjectsMenu();
	/// Clear calculation result in the current CGNS file.
	void clearCalculationResult();
	/// Manage calculated results;
	void manageCalculatedResults();
	void alignSubWindows();
	void copyCameraSetting();

signals:
	void allPostProcessorsUpdated();

private slots:
	void showProjectPropertyDialog();
	/// Cut.
	void cut();
	/// Copy.
	void copy();
	/// Paste.
	void paste();
	/// Take snapshot and save to Raster image (like bmp, jpeg, png).
	void snapshot();
	/// Copy snapshot to clipboard
	void copySnapshot();
	/// Take snapshot and save to SVG image.
	void snapshotSvg();
	/// Take continuous snapshot and save to Raster image.
	void continuousSnapshot();
	/// Import Calculation condition
	void importCalcCondition();
	/// Export Calculation condition
	void exportCalcCondition();
	/// Import Calculation result from CGNS file.
	void importCalculationResult();
	/// Import Measured data from CSV file.
	void importMeasuredData();
	/// Launch tool in guis folder.
	void launchExternalTool();

	void showPreferenceDialog();
	void updatePostActionStatus();

	void openHelp();

private:
	void updateWindowZIndices();
	void reflectWindowZIndices();
	/// Setup PreProcessor and SolverConsoleWindow.
	void setupBasicSubWindows();
	/// Setup Central Widget, that is, MDIArea.
	void setupCentralWidget();
	void setupForNewProjectData();
	/// Update window title depending on the current project data
	void updateWindowTitle();
	void setupAnimationToolbar();
	void setupStatusBar();
	void setupAboutDialog();
	void saveContinuousSnapshot(ContinuousSnapshotWizard* wizard, QXmlStreamWriter* writer = nullptr);
	void addKMLElement(int time, QString url, double north, double south, double west, double east, double angle, QXmlStreamWriter* writer);
	void setupProcessEnvironment();
	void setupNetworkProxy();
	void updateTmsListForAllWindows();

	QString timeString(int time);

	MousePositionWidget* m_mousePositionWidget;
	CoordinateSystemDisplayWidget* m_coordinateSystemWidget;
	VtkGraphicsViewScaleWidget* m_viewScaleWidget;
	Vtk2dGraphicsViewAngleWidget* m_viewAngleWidget;
	QMainWindowWithSnapshotResizeWidget* m_resizeWidget;

	/// Action manager
	iRICMainWindowActionManager* m_actionManager;

	/// Dialog manager
	iRICMainWindowMiscDialogManager* m_miscDialogManager;

	QMenu* m_menuWindow;
	QMenu* m_menuHelp;

	/// Workspace
	ProjectWorkspace* m_workspace;
	/// Project data
	ProjectData* m_projectData;
	/// Locale of current user
	QLocale m_locale;
	/// List of solver definitions
	SolverDefinitionList* m_solverDefinitionList;

	/// List of GUI tools
	SolverDefinitionList* m_guiToolList;

	CoordinateSystemBuilder* m_coordinateSystemBuilder;

	/// List of grid creating programs
	QList<GridCreatingConditionCreator*> m_gridCreatingConditionCreators;
	/// The definition information of the solver currently used
	SolverDefinition* m_solverDefinition;

	AnimationController* m_animationController;

	bool m_isOpening; // true when opening a project file.
	bool m_isSaving; // true when saving a project file.

	VersionNumber m_versionNumber;

	ContinuousSnapshotSetting m_continuousSnapshotSetting;
	bool m_continuousSnapshotInProgress;

	bool m_cuiMode;

	iRICMetaData* m_metaData;

	PostProcessorWindowFactory* m_postWindowFactory;

	bool m_debugMode;

	QProcessEnvironment m_processEnvironment;

	class CalculatedResultManager;
	class ModelessDialogModeChanger;
	class SnapshotSaver;

public:
	friend class ProjectData;
	friend class iRICMainWindowActionManager;
	friend class iRICMainWindowMiscDialogManager;

	friend class iRICPython::Application;
};

#endif // IRICMAINWINDOW_H
