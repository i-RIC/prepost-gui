#ifndef IRICMAINWINDOWACTIONMANAGER_H
#define IRICMAINWINDOWACTIONMANAGER_H

#include <QObject>
#include <QList>
#include <QSignalMapper>
#include <QMap>

#include <guicore/solverdef/solverdefinition.h>
#include <guicore/project/cgnsfilelist.h>

class QMenu;
class QMenuBar;
class QToolBar;
class QAction;
class ProjectData;
class SolverDefinitionList;
class iRICMainWindow;

/// This class manages the actions on iRIC main window,
/// that is an instance of iRICMainWindow.
class iRICMainWindowActionManager : public QObject
{
	Q_OBJECT
public:
	/// Constructor
	iRICMainWindowActionManager(iRICMainWindow* parent);
	/// Return the menubar it manages.
	QMenuBar* menuBar() {return m_menuBar;}
	/// Return the main toolbar.
	QToolBar* mainToolBar() {return m_mainToolBar;}
	/// Set new project data, and create appropriate connections.
	void setProjectData(ProjectData* d);
	/// Set the pointer of animation menu.
	void setAnimationWidgets(QMenu* m, QToolBar* t);
	/// Update the menubar.
	/**
	 * Currently, the menu item that is shown or hidden depending on the situation
	 * is only the "Animation" menu, but maybe in the future, more menus are
	 * shown / hidden in this function.
	 */
	void updateMenuBar();
	void setAdditionalMenus(const QList<QMenu*>& menus) {
		m_additionalMenus = menus;
	}
	QToolBar* additionalToolBar() {return m_additionalToolBar;}
	QToolBar* animationToolbar() {return m_animationToolbar;}
	void unregisterAdditionalToolBar() {
		m_additionalToolBar = nullptr;
	}
	QMenu* recentProjectsMenu() {return m_recentProjectsMenu;}
	void informSubWindowChange(QWidget* subwindow);
	/// Signal mapper to map signals to create new project.
	QSignalMapper* newProjectSignals() {return m_newProjectSignals;}
	/// Action to create new project data
	QAction* newAction;
	/// Action to open project data
	QAction* openAction;
	/// Action to save project data
	QAction* saveAction;
	/// Action to save project data with filename
	QAction* saveAsFileAction;
	/// Action to save project data with foldername
	QAction* saveAsProjectAction;
	/// Action to show Project property dialog
	QAction* propertyAction;
	/// Action to show start dialog
	QAction* showStartDialogAction;
	/// Action to open work folder with Explorer (in Windows).
	QAction* openWorkFolderAction;
	/// Action to exit iRIC
	QAction* exitAction;
	/// Action to save snapshot of current window.
	QAction* saveSnapshotAction;
	/// Action to save snapshots continuously.
	QAction* saveContinuousSnapShotAction;

	/// Action to import CGNS file
	QAction* importCgnsAction;
	QAction* importCgnsActionInCalcMenu;
	/// Action to export CGNS file
	//QAction* exportCgnsAction;
	/// Action to import calculation condition
	QAction* importCalcCondAction;
	/// Action to export calculation condition
	QAction* exportCalcCondAction;
	/// Action to import background image
	QAction* importImageAction;
	/// Action to import background image using Google Map API
	QAction* importGoogleMapImageAction;
	/// Action to import measured values from CSV files (or text files).
	QAction* importMeasuredAction;
	/// Action to import Visualization/Graph window settings
	QAction* importVisGraphAction;
	QAction* importVisGraphActionInCalcMenu;
	/// Action to export Visualization/Graph window settings
	QAction* exportVisGraphAction;
	QAction* exportVisGraphActionInCalcMenu;

	/// Action to undo
	QAction* undoAction;
	/// Action to redo
	QAction* redoAction;
	/// Action to cut
	QAction* cutAction;
	/// Action to copy
	QAction* copyAction;
	/// Action to paste
	QAction* pasteAction;

	/// Action to show/hide the main toolbar
	QAction* viewMainToolBarAction;
	/// Action to show/hide the animation toolbar.
	QAction* viewAnimationToolBarAction;
	/// Action to show/hide the object browser.
	QAction* viewObjectBrowserAction;
	/// Action to show/hide the attribute browser.
	QAction* viewPropertyBrowserAction;
	/// Action to show/hide the status bar
	QAction* viewStatusBarAction;

	/// Action to edit background color
	QAction* viewBackgroundColorAction;

	/// Action to fit
	QAction* cameraFitAction;
	/// Action to reset rotation
	QAction* cameraResetRotationAction;
	/// Action to rotate 90 degree
	QAction* cameraRotate90Action;
	/// Action to view X-Y surface
	QAction* cameraXYPlaneAction;
	/// Action to view Y-Z surface
	QAction* cameraYZPlaneAction;
	/// Action to view Z-X surface
	QAction* cameraZXPlaneAction;
	/// Action to move left
	QAction* cameraMoveLeftAction;
	/// Action to move right
	QAction* cameraMoveRightAction;
	/// Action to move up
	QAction* cameraMoveUpAction;
	/// Action to move down
	QAction* cameraMoveDownAction;
	/// Action to zoom in
	QAction* cameraZoomInAction;
	/// Action to zoom out
	QAction* cameraZoomOutAction;
	/// Action to zoom in in X direction
	QAction* cameraZoomInXAction;
	/// Action to zoom out in X direction
	QAction* cameraZoomOutXAction;
	/// Action to zoom in in Y direction
	QAction* cameraZoomInYAction;
	/// Action to zoom out in Y direction
	QAction* cameraZoomOutYAction;

	/// Action to edit CGNS file list.
	QAction* cgnsEditDialogAction;
	/// Action to run solver
	QAction* solverRunAction;
	/// Action to stop solver
	QAction* solverStopAction;
	/// Action to show dialog about the current solver
	QAction* solverAboutAction;
	/// Action to clear calculation condition.
	QAction* solverClearResultAction;
	/// Action to tile windows inside the m_Center (that is a QMdiArea) of iRICMainWindow.
	QAction* windowTileAction;
	/// Action to set focus on PreProcessorWindow.
	QAction* windowFocusPreProcessorAction;
	/// Action to set focus on SolverConsoleWindow.
	QAction* windowFocusSolverConsoleAction;
	/// Action to open new 2D Post Processor Window.
	QAction* windowCreateNew2dPostProcessorAction;
	/// Action to open new 2D Bird Eye Post Processor Window.
	QAction* windowCreateNew2dBirdEyePostProcessorAction;
	/// Action to open new 3D Post Processor Window.
	QAction* windowCreateNew3dPostProcessorAction;
	/// Action to open new graph window (position x-axis)
//	QAction* windowCreateNewGraph2dPositionWindowAction;
	/// Action to open new graph window (time x-axis)
//	QAction* windowCreateNewGraph2dTimeWindowAction;
	/// Action to open new graph window (hybrid).
	QAction* windowCreateNewGraph2dHybridWindowAction;
	/// Action to open new scattered chart window
	QAction* windowCreateNewGraph2dScatteredWindowAction;
	/// Action to open comparison graph window
	QAction* windowCreateVerificationDialogAction;

	/// Action to open new Graph Window.
	QAction* windowCreateNewGraphAction;
	/// Action to export Calculation Result. (In file menu);
	QAction* calcResultExportActionInFileMenu;
	/// Action to export Calculation Result. (In calculation result menu);
	QAction* calcResultExportActionInCalcMenu;
	/// Action to export Particles data into VTK files
	QAction* particleExportAction;
	/// Action to export Google Earth KML for street view.
	QAction* svKmlExportAction;
	/// Action to reload Calculation Result.
	QAction* calcResultReloadAction;
	QAction* calcResultReloadToolBarAction;

	/// Action to show language setting.
	QAction* optionPreferencesAction;
	/// Action to show maintainance dialog.
	QAction* optionMaintainanceAction;

	/// Action to edit Z direction scale
	QAction* viewZDirectionScaleAction;

	/// Action to check a solver definition file
	QAction* checkDefinitionAction;
	/// Action to show solver definition file translation update wizard
	QAction* translationUpdateAction;

	/// Action to show iRIC help
	QAction* helpAction;
	/// Action to show SolverDefinitionListDialog
	QAction* solverDefinitionListDialogAction;
	/// Action to show Mouse operation dialog
	QAction* aboutMouseAction;
	/// Action to show "About Dialog" of iRIC
	QAction* aboutAction;

public slots:
	/// Project file is opened. It should enable actions only available
	/// when a project file is opened.
	void projectFileOpen();
	/// Project file is closed. It should disable actions only available
	/// when a project file is opened.
	void projectFileClose();
	/// Focus is moved to preProcessorWindow
	void focusPreProcessorWindow() {}
	/// Leave focus from PreProcessorWindow
	void leavePreProcessorWindow() {}
	/// List of solvers that appears in "File" menu is updated.
	void updateSolverList(SolverDefinitionList*);
	void updateAdditionalMenus(const QList<QMenu*>& menus);
	void handleSolverStart();
	void handleSolverFinish();

private slots:
	/// Setup import menu
	void setupImportMenu();
	/// Setup export menu
	void setupExportMenu();
	void updateWindowList();
	void activateWindow(QWidget* w);

private:
	/// Initialization
	/**
	 * Initially, iRIC doesn't open project file, so menus
	 * available only when project file is opened, are disabled.
	 */
	void init();
	/// Setup file menu
	void setupFileMenu();
	/// Setup edit menu
	void setupEditMenu();
	/// Setup view menu
	void setupViewMenu();
	/// Setup setting menu
	void setupSettingMenu();
	/// Setup camera control menu
	void setupCameraControlMenu();

	void updateGridCreationModeMenu();
	/// Setup solver menu
	void setupSimulationMenu();
	/// Setup window menu
	void setupWindowMenu();
	/// Setup tool menu
	void setupToolMenu();
	/// Setup calculation result menu
	void setupCalculationResultMenu();
	/// Setup help menu
	void setupHelpMenu();
	/// Setup main toolbar
	void setupMainToolBar();

	void addGridImportMenu(QMenu* menu);
	void addGridExportMenu(QMenu* menu);

	void updateCameraConnections(QWidget* w);
	void updateMiscActions(QWidget* w);
	void updateObjectBrowserMenu(QWidget* w);
	void updatePropertyBrowserMenu(QWidget* w);
	static void updateCameraAction(QAction* a, QWidget* w, const char* member);

	static void enableActions(const QList<QAction*>& actions, bool enable);
	static void uncheckActions(const QList<QAction*>& actions);

	QMenuBar* m_menuBar;
	/// Signal mapper to map signals to create new project.
	QSignalMapper* m_newProjectSignals;
//	QSignalMapper* m_gridModeSignals;
//	QMap<SolverDefinition::GridType, QAction*> m_gridModeActions;
	/// File menu
	QMenu* m_fileMenu;
	/// Edit menu
	QMenu* m_editMenu;
	/// View menu
	QMenu* m_viewMenu;
	/// Camera control menu
	QMenu* m_cameraControlMenu;
	/// Setting menu
	QMenu* m_optionMenu;
	/// Simulation menu
	QMenu* m_simulationMenu;
	/// Animation menu
	/**
	 * Note that animation menu is not created in this
	 * class, but in AnimationController.
	 */
	QMenu* m_animationMenu;

	/// Calculation result menu
	QMenu* m_resultMenu;
	/// Tool menu
	QMenu* m_toolMenu;
	/// Help menu
	QMenu* m_helpMenu;

	/// Menu to create new Projects.
	QMenu* m_newProjectMenu;
	/// Menu to import data
	QMenu* m_importMenu;
	QMenu* m_importMenuInFileMenu;
	/// Menu to export data
	QMenu* m_exportMenu;
	/// Menu to load recently opened projects.
	QMenu* m_recentProjectsMenu;

	QMenu* m_gridImportMenu;
	QMenu* m_gridExportMenu;

	QMenu* m_rawDataImportMenu;
	QMenu* m_rawDataExportMenu;

	QMenu* m_hydraulicDataImportMenu;

	QAction* m_windowMenuSeparetor;

	QSignalMapper* m_windowActivationMapper;

	QList<QMenu*> m_additionalMenus;

	QToolBar* m_mainToolBar;

	QToolBar* m_additionalToolBar;
	/**
	 * Note that animation toolbar is not created in this
	 * class, but in AnimationController.
	 */
	QToolBar* m_animationToolbar;

	bool m_isPostWindowActive;
	bool m_isProjectFileOpen;
	iRICMainWindow* m_parent;
};

#endif // IRICMAINWINDOWACTIONMANAGER_H
