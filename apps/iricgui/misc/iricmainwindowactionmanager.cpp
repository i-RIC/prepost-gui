#include "../main/iricmainwindow.h"
#include "../solverdef/solverdefinitionlist.h"
#include "iricmainwindowactionmanager.h"
#include "iricmainwindowmiscdialogmanager.h"

#include <guibase/irictoolbar.h>
#include <guibase/objectbrowser.h>
#include <guicore/base/propertybrowser.h>
#include <guicore/base/windowwithobjectbrowser.h>
#include <guicore/post/postprocessorwindow.h>
#include <guicore/project/cgnsfilelist.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/solverdef/solverdefinition.h>
#include <guicore/solverdef/solverdefinitionabstract.h>
#include <misc/iricundostack.h>
#include <pre/preprocessorwindow.h>
#include <solverconsole/solverconsolewindow.h>

#include <QAction>
#include <QComboBox>
#include <QList>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMenuBar>
#include <QStatusBar>

iRICMainWindowActionManager::iRICMainWindowActionManager(iRICMainWindow* parent)
{
	m_parent = parent;
	m_additionalToolBar = 0;
	init();
}

void iRICMainWindowActionManager::init(){
	m_isPostWindowActive = false;
	m_isProjectFileOpen = false;
	m_menuBar = new QMenuBar(m_parent);

	setupFileMenu();
	setupEditMenu();
	setupViewMenu();
	setupCameraControlMenu();

	setupSimulationMenu();

	m_animationMenu = 0;
	m_animationToolbar = 0;

	setupToolMenu();

	setupWindowMenu();
	setupCalculationResultMenu();
	setupSettingMenu();
	setupHelpMenu();

	updateMenuBar();
	setupMainToolBar();
//	setupCameraControlToolBar();
}

void iRICMainWindowActionManager::setupFileMenu()
{
	m_fileMenu = new QMenu(tr("&File"), m_menuBar);

	newAction = new QAction(tr("&New Project..."), m_fileMenu);
	newAction->setIcon(QIcon(":/images/iconNew.png"));
	newAction->setShortcut(QKeySequence(tr("Ctrl+N")));

	m_fileMenu->addAction(newAction);
	connect(newAction, SIGNAL(triggered()), m_parent, SLOT(newProject()));

	openAction = new QAction(tr("&Open..."), m_fileMenu);
	openAction->setIcon(QIcon(QString(":/images/iconOpen.png")));
	openAction->setShortcut(QKeySequence(tr("Ctrl+O")));
	openAction->setStatusTip(tr("Open iRIC Project files"));
	m_fileMenu->addAction(openAction);

	connect(openAction, SIGNAL(triggered()), m_parent, SLOT(openProject()));

	saveAction = new QAction(tr("&Save"), m_fileMenu);
	saveAction->setIcon(QIcon(":/images/iconSave.png"));
	saveAction->setShortcut(QKeySequence(tr("Ctrl+S")));
	saveAction->setDisabled(true);
	m_fileMenu->addAction(saveAction);
	connect(saveAction, SIGNAL(triggered()), m_parent, SLOT(saveProject()));

	saveAsFileAction = new QAction(tr("Save &As File(*.ipro)..."), m_fileMenu);
	saveAsFileAction->setDisabled(true);
	m_fileMenu->addAction(saveAsFileAction);
	connect(saveAsFileAction, SIGNAL(triggered()), m_parent, SLOT(saveProjectAsFile()));

	saveAsProjectAction = new QAction(tr("Save As &Project..."), m_fileMenu);
	saveAsProjectAction->setDisabled(true);
	m_fileMenu->addAction(saveAsProjectAction);
	connect(saveAsProjectAction, SIGNAL(triggered()), m_parent, SLOT(saveProjectAsFolder()));

	m_fileMenu->addSeparator();

	propertyAction = new QAction(tr("&Property..."), m_fileMenu);
	propertyAction->setIcon(QIcon(":/libs/guibase/images/iconProperty.png"));
	propertyAction->setDisabled(true);
	m_fileMenu->addAction(propertyAction);
	connect(propertyAction, SIGNAL(triggered()), m_parent, SLOT(showProjectPropertyDialog()));

	m_fileMenu->addSeparator();

	saveSnapshotAction = new QAction(tr("Save S&napshot..."), m_fileMenu);
	saveSnapshotAction->setIcon(QIcon(":/images/iconSnapshot.png"));
	saveSnapshotAction->setDisabled(true);
	m_fileMenu->addAction(saveSnapshotAction);
	connect(saveSnapshotAction, SIGNAL(triggered()), m_parent, SLOT(snapshot()));

	saveContinuousSnapShotAction = new QAction(tr("Continuous Snapshot / Movie / Google Earth Export..."), m_fileMenu);
	saveContinuousSnapShotAction->setIcon(QIcon(":/images/iconGoogleEarth.png"));
	saveContinuousSnapShotAction->setDisabled(true);
	m_fileMenu->addAction(saveContinuousSnapShotAction);
	connect(saveContinuousSnapShotAction, SIGNAL(triggered()), m_parent, SLOT(continuousSnapshot()));

	m_fileMenu->addSeparator();

	m_importMenu = new QMenu(tr("&Import"), m_fileMenu);
	m_importMenuInFileMenu = new QMenu(tr("&Import"), m_menuBar);
	m_importMenuInFileMenu->setIcon(QIcon(":/libs/guibase/images/iconImport.png"));
	m_fileMenu->addMenu(m_importMenuInFileMenu);
	m_importMenu->setDisabled(true);
	m_importMenuInFileMenu->setDisabled(true);
	connect(m_importMenu, SIGNAL(aboutToShow()), this, SLOT(setupImportMenu()));
	connect(m_importMenuInFileMenu, SIGNAL(aboutToShow()), this, SLOT(setupImportMenu()));
	m_rawDataImportMenu = 0;

	importCalcCondAction = new QAction(tr("&Calculation Condition..."), this);
	importCgnsAction = new QAction(tr("Calculation &Result..."), this);
	connect(importCgnsAction, SIGNAL(triggered()), m_parent, SLOT(importCalculationResult()));

	importMeasuredAction = new QAction(tr("&Measured Values..."), this);
	connect(importMeasuredAction, SIGNAL(triggered()), m_parent, SLOT(importMeasuredData()));

	importImageAction = new QAction(tr("&Background Image..."), this);

	importGoogleMapImageAction = new QAction(tr("&Google Map Background Image..."), this);
	connect(importGoogleMapImageAction, SIGNAL(triggered()), m_parent, SLOT(importGoogleMapImages()));

	importVisGraphAction = new QAction(tr("&Visualization/Graph Settings..."), this);
	connect(importVisGraphAction, SIGNAL(triggered()), m_parent, SLOT(importVisGraphSetting()));

	exportVisGraphAction = new QAction(tr("&Visualization/Graph Settings..."), this);
	connect(exportVisGraphAction, SIGNAL(triggered()), m_parent, SLOT(exportVisGraphSetting()));

	m_exportMenu = new QMenu(tr("&Export"), m_fileMenu);
	m_exportMenu->setIcon(QIcon(":/libs/guibase/images/iconExport.png"));
	m_fileMenu->addMenu(m_exportMenu);
	m_exportMenu->setDisabled(true);
	connect(m_exportMenu, SIGNAL(aboutToShow()), this, SLOT(setupExportMenu()));

	exportCalcCondAction = new QAction(tr("&Calculation Condition..."), this);
	calcResultExportActionInFileMenu = new QAction(tr("Calculation &Result..."), this);
	connect(calcResultExportActionInFileMenu, SIGNAL(triggered()), m_parent, SLOT(exportCalculationResult()));
	particleExportAction = new QAction(tr("&Particles..."), this);
	connect(particleExportAction, SIGNAL(triggered()), m_parent, SLOT(exportParticles()));
	svKmlExportAction = new QAction(tr("&Google Earth KML for street view"), this);
	connect(svKmlExportAction, SIGNAL(triggered()), m_parent, SLOT(exportStKML()));

	m_fileMenu->addSeparator();

	// open work folder action is implemented, but currently hidden.

	openWorkFolderAction = new QAction(tr("Open &Work Directory..."), this);
	openWorkFolderAction->setDisabled(true);
//	m_fileMenu->addAction(openWorkFolderAction);

//	m_fileMenu->addSeparator();

	m_recentProjectsMenu = new QMenu(tr("&Recent Projects"), m_fileMenu);
	connect(m_recentProjectsMenu, SIGNAL(aboutToShow()), m_parent, SLOT(setupRecentProjectsMenu()));
	m_fileMenu->addMenu(m_recentProjectsMenu);

	m_fileMenu->addSeparator();

	showStartDialogAction = new QAction(tr("Show iRIC Start Page..."), this);
	m_fileMenu->addAction(showStartDialogAction);
	connect(showStartDialogAction, SIGNAL(triggered()), m_parent, SLOT(openStartDialog()));

	m_fileMenu->addSeparator();

	exitAction = new QAction(tr("E&xit"), m_fileMenu);
	m_fileMenu->addAction(exitAction);
	connect(exitAction, SIGNAL(triggered()), m_parent, SLOT(close()));
}

void iRICMainWindowActionManager::setupImportMenu()
{
	m_importMenu->clear();
	m_rawDataImportMenu = m_importMenu->addMenu(tr("G&eographic Data"));
	connect(m_rawDataImportMenu, SIGNAL(aboutToShow()), m_parent->preProcessorWindow(), SLOT(setupRawDataImportMenu()));

	m_hydraulicDataImportMenu = m_importMenu->addMenu(tr("&Hydraulic Data"));
	connect(m_hydraulicDataImportMenu, SIGNAL(aboutToShow()), m_parent->preProcessorWindow(), SLOT(setupHydraulicDataImportMenu()));

	addGridImportMenu(m_importMenu);
	m_importMenu->addAction(importCalcCondAction);
	m_importMenu->addAction(importCgnsAction);
	m_importMenu->addAction(importMeasuredAction);
	m_importMenu->addAction(importImageAction);
	m_importMenu->addAction(importGoogleMapImageAction);
	m_importMenu->addAction(importVisGraphAction);

	m_importMenuInFileMenu->clear();
	m_rawDataImportMenu = m_importMenuInFileMenu->addMenu(tr("G&eographic Data"));
	connect(m_rawDataImportMenu, SIGNAL(aboutToShow()), m_parent->preProcessorWindow(), SLOT(setupRawDataImportMenu()));

	m_hydraulicDataImportMenu = m_importMenuInFileMenu->addMenu(tr("&Hydraulic Data"));
	connect(m_hydraulicDataImportMenu, SIGNAL(aboutToShow()), m_parent->preProcessorWindow(), SLOT(setupHydraulicDataImportMenu()));

	addGridImportMenu(m_importMenuInFileMenu);
	m_importMenuInFileMenu->addAction(importCalcCondAction);
	m_importMenuInFileMenu->addAction(importCgnsAction);
	m_importMenuInFileMenu->addAction(importMeasuredAction);
	m_importMenuInFileMenu->addAction(importImageAction);
	m_importMenuInFileMenu->addAction(importGoogleMapImageAction);
	m_importMenuInFileMenu->addAction(importVisGraphAction);
}

void iRICMainWindowActionManager::setupExportMenu()
{
	m_exportMenu->clear();
	m_rawDataExportMenu = m_exportMenu->addMenu(tr("G&eographic Data"));
	connect(m_rawDataExportMenu, SIGNAL(aboutToShow()), m_parent->preProcessorWindow(), SLOT(setupRawDataExportMenu()));
	addGridExportMenu(m_exportMenu);
//	exportCgnsAction = new QAction(tr("&Case (CGNS file)..."), m_exportMenu);
//	m_exportMenu->addAction(exportCgnsAction);
	m_exportMenu->addAction(exportCalcCondAction);

	QAction* exportLogAction = new QAction(tr("Solver Console &Log..."), m_exportMenu);
	connect(exportLogAction, SIGNAL(triggered()), m_parent->solverConsoleWindow()->exportLogAction, SIGNAL(triggered()));
	exportLogAction->setEnabled(m_parent->solverConsoleWindow()->exportLogAction->isEnabled());
	m_exportMenu->addAction(exportLogAction);
	m_exportMenu->addAction(calcResultExportActionInFileMenu);
	m_exportMenu->addAction(particleExportAction);
	m_exportMenu->addAction(svKmlExportAction);
	m_exportMenu->addAction(exportVisGraphAction);
}

void iRICMainWindowActionManager::setupEditMenu()
{
	m_editMenu = new QMenu(tr("&Edit"), m_menuBar);

	undoAction = iRICUndoStack::instance().createUndoAction(m_editMenu);
	undoAction->setIcon(QIcon(":/images/iconUndo.png"));
	undoAction->setShortcut(QKeySequence(tr("Ctrl+Z")));
	undoAction->setShortcutContext(Qt::ApplicationShortcut);
	m_editMenu->addAction(undoAction);

	redoAction = iRICUndoStack::instance().createRedoAction(m_editMenu);
	redoAction->setIcon(QIcon(":/images/iconRedo.png"));
	redoAction->setShortcut(QKeySequence(tr("Ctrl+Y")));
	redoAction->setShortcutContext(Qt::ApplicationShortcut);
	m_editMenu->addAction(redoAction);

	m_editMenu->addSeparator();

	cutAction = new QAction(tr("Cu&t"), m_editMenu);
	cutAction->setIcon(QIcon(":/images/iconCut.png"));
	m_editMenu->addAction(cutAction);
	connect(cutAction, SIGNAL(triggered()), m_parent, SLOT(cut()));

	copyAction = new QAction(tr("&Copy"), m_editMenu);
	copyAction->setIcon(QIcon(":/images/iconCopy.png"));
	m_editMenu->addAction(copyAction);
	connect(copyAction, SIGNAL(triggered()), m_parent, SLOT(copy()));

	pasteAction = new QAction(tr("&Paste"), m_editMenu);
	pasteAction->setIcon(QIcon(":/images/iconPaste.png"));
	m_editMenu->addAction(pasteAction);
	connect(pasteAction, SIGNAL(triggered()), m_parent, SLOT(paste()));
}

void iRICMainWindowActionManager::setupViewMenu()
{
	m_viewMenu = new QMenu(tr("&View"), m_menuBar);
	connect(m_viewMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowList()));

	QMenu* toolBarMenu = new QMenu(tr("&Toolbar"), m_viewMenu);
	m_viewMenu->addMenu(toolBarMenu);

	viewMainToolBarAction = new QAction(tr("&Main Toolbar"), toolBarMenu);
	viewMainToolBarAction->setCheckable(true);
	viewMainToolBarAction->setChecked(true);
	toolBarMenu->addAction(viewMainToolBarAction);

	viewAnimationToolBarAction = new QAction(tr("&Animation Toolbar"), toolBarMenu);
	viewAnimationToolBarAction->setCheckable(true);
	viewMainToolBarAction->setChecked(true);
	toolBarMenu->addAction(viewAnimationToolBarAction);

	viewObjectBrowserAction = new QAction(tr("&Object Browser"), m_viewMenu);
	viewObjectBrowserAction->setCheckable(true);
	m_viewMenu->addAction(viewObjectBrowserAction);

	viewPropertyBrowserAction = new QAction(tr("&Attribute Browser"), m_viewMenu);
	viewPropertyBrowserAction->setCheckable(true);
	m_viewMenu->addAction(viewPropertyBrowserAction);

	viewStatusBarAction = new QAction(tr("&Status Bar"), m_viewMenu);
	viewStatusBarAction->setCheckable(true);
	viewStatusBarAction->setChecked(true);
	m_viewMenu->addAction(viewStatusBarAction);

	connect(viewStatusBarAction, SIGNAL(triggered(bool)), m_parent->statusBar(), SLOT(setVisible(bool)));

	m_viewMenu->addSeparator();

	viewBackgroundColorAction = new QAction(tr("&Background Color..."), m_viewMenu);
	viewBackgroundColorAction->setIcon(QIcon(":/libs/guibase/images/iconColor.png"));
	m_viewMenu->addAction(viewBackgroundColorAction);
	viewBackgroundColorAction->setDisabled(true);

	viewZDirectionScaleAction = new QAction(tr("&Z-direction Scale..."), m_viewMenu);
	m_viewMenu->addAction(viewZDirectionScaleAction);
	viewZDirectionScaleAction->setDisabled(true);

	m_viewMenu->addSeparator();

	windowTileAction = new QAction(tr("&Tile Windows"), m_viewMenu);
	m_viewMenu->addAction(windowTileAction);
	connect(windowTileAction, SIGNAL(triggered()), m_parent, SLOT(tileSubWindows()));

	m_windowMenuSeparetor = m_viewMenu->addSeparator();

	m_windowActivationMapper = new QSignalMapper(this);
	connect(m_windowActivationMapper, SIGNAL(mapped(QWidget*)), this, SLOT(activateWindow(QWidget*)));
}

void iRICMainWindowActionManager::setupCameraControlMenu()
{
	m_cameraControlMenu = new QMenu(tr("Ca&mera Control"), m_menuBar);

	cameraFitAction = new QAction(tr("&Fit"), m_cameraControlMenu);
	cameraFitAction->setIcon(QIcon(":/images/iconViewFit.png"));

	cameraResetRotationAction = new QAction(tr("&Reset Rotation"), m_cameraControlMenu);
	cameraResetRotationAction->setIcon(QIcon(":/images/iconViewRotateReset.png"));

	cameraRotate90Action = new QAction(tr("R&otate 90"), m_cameraControlMenu);
	cameraRotate90Action->setIcon(QIcon(":/images/iconViewRotate90.png"));

	cameraXYPlaneAction = new QAction(tr("XY Plane"), m_cameraControlMenu);
	cameraXYPlaneAction->setIcon(QIcon(":/images/iconViewXY.png"));

	cameraYZPlaneAction = new QAction(tr("YZ Plane"), m_cameraControlMenu);
	cameraYZPlaneAction->setIcon(QIcon(":/images/iconViewYZ.png"));

	cameraZXPlaneAction = new QAction(tr("XZ Plane"), m_cameraControlMenu);
	cameraZXPlaneAction->setIcon(QIcon(":/images/iconViewXZ.png"));

	cameraMoveLeftAction = new QAction(tr("Move &Left"), m_cameraControlMenu);
	cameraMoveLeftAction->setIcon(QIcon(":/images/iconViewMoveLeft.png"));

	cameraMoveRightAction = new QAction(tr("Move &Right"), m_cameraControlMenu);
	cameraMoveRightAction->setIcon(QIcon(":/images/iconViewMoveRight.png"));

	cameraMoveUpAction = new QAction(tr("Move &Up"), m_cameraControlMenu);
	cameraMoveUpAction->setIcon(QIcon(":/images/iconViewMoveUp.png"));

	cameraMoveDownAction = new QAction(tr("Move &Down"), m_cameraControlMenu);
	cameraMoveDownAction->setIcon(QIcon(":/images/iconViewMoveDown.png"));

	cameraZoomInAction = new QAction(tr("Zoom &In"), m_cameraControlMenu);
	cameraZoomInAction->setIcon(QIcon(":/images/iconViewZoomIn.png"));

	cameraZoomOutAction = new QAction(tr("Zoom &Out"), m_cameraControlMenu);
	cameraZoomOutAction->setIcon(QIcon(":/images/iconViewZoomOut.png"));

	cameraZoomInXAction = new QAction(tr("Zoom In in X Direction"), m_cameraControlMenu);
	cameraZoomInXAction->setIcon(QIcon(":/images/iconViewZoomInX.png"));

	cameraZoomOutXAction = new QAction(tr("Zoom Out in X Direction"), m_cameraControlMenu);
	cameraZoomOutXAction->setIcon(QIcon(":/images/iconViewZoomOutX.png"));

	cameraZoomInYAction = new QAction(tr("Zoom In in Y Direction"), m_cameraControlMenu);
	cameraZoomInYAction->setIcon(QIcon(":/images/iconViewZoomInY.png"));

	cameraZoomOutYAction = new QAction(tr("Zoom Out in Y Direction"), m_cameraControlMenu);
	cameraZoomOutYAction->setIcon(QIcon(":/images/iconViewZoomOutY.png"));
}

void iRICMainWindowActionManager::setupSimulationMenu()
{
	m_simulationMenu = new QMenu(tr("&Simulation"), m_menuBar);

	solverRunAction = new QAction(tr("&Run"), m_simulationMenu);
	solverRunAction->setIcon(QIcon(":/images/iconSolverRun.png"));
	solverRunAction->setShortcut(QKeySequence(tr("Ctrl+R")));
	m_simulationMenu->addAction(solverRunAction);
	connect(solverRunAction, SIGNAL(triggered()), m_parent->solverConsoleWindow(), SLOT(startSolver()));

	solverStopAction = new QAction(tr("&Stop", "stop solver"), m_simulationMenu);
	solverStopAction->setIcon(QIcon(":/images/iconSolverStop.png"));
	m_simulationMenu->addAction(solverStopAction);
	connect(solverStopAction, SIGNAL(triggered()), m_parent->solverConsoleWindow(), SLOT(terminateSolver()));

	m_simulationMenu->addSeparator();

	solverAboutAction = new QAction(tr("&Solver Information..."), m_simulationMenu);
	solverAboutAction->setIcon(QIcon(":/libs/solverconsole/images/iconSolver.png"));
	m_simulationMenu->addAction(solverAboutAction);
	connect(solverAboutAction, SIGNAL(triggered()), m_parent->m_miscDialogManager, SLOT(aboutCurrentSolver()));

	m_simulationMenu->addSeparator();

	m_simulationMenu->addAction(m_parent->solverConsoleWindow()->exportLogAction);

	cgnsEditDialogAction = new QAction(tr("&Edit case list..."), m_simulationMenu);
//	m_simulationMenu->addAction(cgnsEditDialogAction);

	connect(m_parent->solverConsoleWindow(), SIGNAL(solverStarted()), this, SLOT(handleSolverStart()));
	connect(m_parent->solverConsoleWindow(), SIGNAL(solverFinished()), this, SLOT(handleSolverFinish()));

	m_simulationMenu->setEnabled(false);
	enableActions(m_simulationMenu->actions(), false);
}

void iRICMainWindowActionManager::setupWindowMenu()
{
//	m_windowMenu = new QMenu(tr("&Window"), m_menuBar);
//	connect(m_windowMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowList()));

	windowFocusPreProcessorAction = new QAction(tr("Focus &PreProcessor Window"), this);
	windowFocusPreProcessorAction->setIcon(QIcon(":/images/iconPreprocessing.png"));
	connect(windowFocusPreProcessorAction, SIGNAL(triggered()), m_parent, SLOT(focusPreProcessorWindow()));
	windowFocusPreProcessorAction->setDisabled(true);

	windowFocusSolverConsoleAction = new QAction(tr("Focus &Solver Console Window"), this);
	windowFocusSolverConsoleAction->setIcon(QIcon(":/libs/solverconsole/images/iconSolver.png"));
	connect(windowFocusSolverConsoleAction, SIGNAL(triggered()), m_parent, SLOT(focusSolverConsoleWindow()));
	windowFocusSolverConsoleAction->setDisabled(true);

}

void iRICMainWindowActionManager::setupToolMenu()
{
	m_toolMenu = new QMenu(tr("&Tool"), m_menuBar);

	//checkDefinitionAction = new QAction(tr("&Check solver definition file..."), m_toolMenu);
	//m_toolMenu->addAction(checkDefinitionAction);
	// this menu is not enabled yet.

	//translationUpdateAction = new QAction(tr("&Create/update translation files..."), m_toolMenu);
	//translationUpdateAction->setIcon(QIcon(":/images/iconDictionary.png"));

	//m_toolMenu->addAction(translationUpdateAction);
	//connect(translationUpdateAction, SIGNAL(triggered()), m_parent->m_miscDialogManager, SLOT(solverDefinitionTranslationWizard()));
}

void iRICMainWindowActionManager::setupCalculationResultMenu()
{
	m_resultMenu = new QMenu(tr("Calculation &Result"), m_menuBar);

	windowCreateNew2dPostProcessorAction = new QAction(tr("Open new 2D Post-Processing Window"), m_resultMenu);
	windowCreateNew2dPostProcessorAction->setIcon(QIcon(":/libs/post/post2d/images/iconVis2D.png"));
	m_resultMenu->addAction(windowCreateNew2dPostProcessorAction);
	connect(windowCreateNew2dPostProcessorAction, SIGNAL(triggered()), m_parent, SLOT(create2dPostWindow()));

	windowCreateNew2dBirdEyePostProcessorAction = new QAction(tr("Open new 2D Bird's-Eye Post-Processing Window"), m_resultMenu);
	windowCreateNew2dBirdEyePostProcessorAction->setIcon(QIcon(":/libs/post/post2dbirdeye/images/iconVis2DBirdEye.png"));
	m_resultMenu->addAction(windowCreateNew2dBirdEyePostProcessorAction);
	connect(windowCreateNew2dBirdEyePostProcessorAction, SIGNAL(triggered()), m_parent, SLOT(create2dBirdEyePostWindow()));

	windowCreateNew3dPostProcessorAction = new QAction(tr("Open new 3D Post-Processing Window"), m_resultMenu);
	windowCreateNew3dPostProcessorAction->setIcon(QIcon(":/libs/post/post3d/images/iconVis3D.png"));
	m_resultMenu->addAction(windowCreateNew3dPostProcessorAction);
	connect(windowCreateNew3dPostProcessorAction, SIGNAL(triggered()), m_parent, SLOT(create3dPostWindow()));

	m_resultMenu->addSeparator();
/*
	windowCreateNewGraph2dPositionWindowAction = new QAction(tr("Open new Graph Window (Position X-axis)"), m_resultMenu);
	windowCreateNewGraph2dPositionWindowAction->setIcon(QIcon(":/images/iconVisGraphPosition.png"));
	m_resultMenu->addAction(windowCreateNewGraph2dPositionWindowAction);
	connect(windowCreateNewGraph2dPositionWindowAction, SIGNAL(triggered()), m_parent, SLOT(createGraph2dPositionWindow()));
*/
/*
	windowCreateNewGraph2dTimeWindowAction = new QAction(tr("Open new Graph Window (Time X-axis)"), m_resultMenu);
	windowCreateNewGraph2dTimeWindowAction->setIcon(QIcon(":/images/iconVisGraphTime.png"));
	m_resultMenu->addAction(windowCreateNewGraph2dTimeWindowAction);
	connect(windowCreateNewGraph2dTimeWindowAction, SIGNAL(triggered()), m_parent, SLOT(createGraph2dTimeWindow()));
*/
	windowCreateNewGraph2dHybridWindowAction = new QAction(tr("Open new Graph Window"), m_resultMenu);
	windowCreateNewGraph2dHybridWindowAction->setIcon(QIcon(":/images/iconVisGraphHybrid.png"));
	m_resultMenu->addAction(windowCreateNewGraph2dHybridWindowAction);
	connect(windowCreateNewGraph2dHybridWindowAction, SIGNAL(triggered()), m_parent, SLOT(createGraph2dHybridWindow()));

	windowCreateNewGraph2dScatteredWindowAction = new QAction(tr("Open new Scattered Chart Window"), m_resultMenu);
	windowCreateNewGraph2dScatteredWindowAction->setIcon(QIcon(":/images/iconVisGraphScattered.png"));
	m_resultMenu->addAction(windowCreateNewGraph2dScatteredWindowAction);
	connect(windowCreateNewGraph2dScatteredWindowAction, SIGNAL(triggered()), m_parent, SLOT(createGraph2dScatteredWindow()));

	windowCreateVerificationDialogAction = new QAction(tr("Compare with measured values..."), m_resultMenu);
	windowCreateVerificationDialogAction->setIcon(QIcon(":/images/iconGraphVerification.png"));
	m_resultMenu->addAction(windowCreateVerificationDialogAction);
	connect(windowCreateVerificationDialogAction, SIGNAL(triggered()), m_parent, SLOT(openVerificationDialog()));

	m_resultMenu->addSeparator();

	calcResultReloadAction = new QAction(tr("&Reload"), m_resultMenu);
	calcResultReloadAction->setIcon(QIcon(":/images/iconReload.png"));
	calcResultReloadToolBarAction = new QAction(tr("&Reload Calculation Result"), m_resultMenu);
	calcResultReloadToolBarAction->setIcon(QIcon(":/images/iconReload.png"));
	m_resultMenu->addAction(calcResultReloadAction);
	connect(calcResultReloadAction, SIGNAL(triggered()), m_parent, SLOT(checkCgnsStepsUpdate()));
	connect(calcResultReloadToolBarAction, SIGNAL(triggered()), m_parent, SLOT(checkCgnsStepsUpdate()));

	solverClearResultAction = new QAction(tr("&Delete..."), m_resultMenu);
	solverClearResultAction->setIcon(QIcon(":/libs/guibase/images/iconDeleteItem.png"));
	m_resultMenu->addAction(solverClearResultAction);
	connect(solverClearResultAction, SIGNAL(triggered()), m_parent, SLOT(clearCalculationResult()));

	m_resultMenu->addSeparator();

	importCgnsActionInCalcMenu = new QAction(tr("&Import..."), m_resultMenu);
	importCgnsActionInCalcMenu->setIcon(QIcon(":/libs/guibase/images/iconImport.png"));
	connect(importCgnsActionInCalcMenu, SIGNAL(triggered()), m_parent, SLOT(importCalculationResult()));
	m_resultMenu->addAction(importCgnsActionInCalcMenu);

	calcResultExportActionInCalcMenu = new QAction(tr("&Export..."), m_resultMenu);
	calcResultExportActionInCalcMenu->setIcon(QIcon(":/libs/guibase/images/iconExport.png"));
	connect(calcResultExportActionInCalcMenu, SIGNAL(triggered()), m_parent, SLOT(exportCalculationResult()));
	m_resultMenu->addAction(calcResultExportActionInCalcMenu);

	m_resultMenu->addSeparator();

	importVisGraphActionInCalcMenu = new QAction(tr("Import Visualization/Graph Settings..."), m_resultMenu);
	importVisGraphActionInCalcMenu->setIcon(QIcon(":/libs/guibase/images/iconImport.png"));
	connect(importVisGraphActionInCalcMenu, SIGNAL(triggered()), m_parent, SLOT(importVisGraphSetting()));
	m_resultMenu->addAction(importVisGraphActionInCalcMenu);

	exportVisGraphActionInCalcMenu = new QAction(tr("Export Visualization/Graph Settings..."), m_resultMenu);
	exportVisGraphActionInCalcMenu->setIcon(QIcon(":/libs/guibase/images/iconExport.png"));
	connect(exportVisGraphActionInCalcMenu, SIGNAL(triggered()), m_parent, SLOT(exportVisGraphSetting()));
	m_resultMenu->addAction(exportVisGraphActionInCalcMenu);

	enableActions(m_resultMenu->actions(), false);
	m_resultMenu->setEnabled(false);
}

void iRICMainWindowActionManager::setupSettingMenu()
{
	m_optionMenu = new QMenu(tr("&Option"), m_menuBar);

	optionPreferencesAction = new QAction(tr("&Preferences..."), m_optionMenu);
	m_optionMenu->addAction(optionPreferencesAction);
	connect(optionPreferencesAction, SIGNAL(triggered()), m_parent, SLOT(showPreferenceDialog()));

	m_optionMenu->addSeparator();

	optionMaintainanceAction = new QAction(tr("&Maintainance"), m_optionMenu);
	m_optionMenu->addAction(optionMaintainanceAction);
	connect(optionMaintainanceAction, SIGNAL(triggered()), m_parent->m_miscDialogManager, SLOT(showMaintainanceDialog()));

	m_optionMenu->addSeparator();

	translationUpdateAction = new QAction(tr("&Create/Update Translation Files..."), m_optionMenu);
	translationUpdateAction->setIcon(QIcon(":/images/iconDictionary.png"));
	m_optionMenu->addAction(translationUpdateAction);
	connect(translationUpdateAction, SIGNAL(triggered()), m_parent->m_miscDialogManager, SLOT(solverDefinitionTranslationWizard()));
}

void iRICMainWindowActionManager::setupHelpMenu()
{
	m_helpMenu = new QMenu(tr("&Help"), m_menuBar);

	helpAction = new QAction(tr("&Help..."), m_helpMenu);
	m_helpMenu->addAction(helpAction);

	solverDefinitionListDialogAction = new QAction(tr("&Solver List..."), m_helpMenu);
	solverDefinitionListDialogAction->setIcon(QIcon(":/libs/solverconsole/images/iconSolver.png"));
	m_helpMenu->addAction(solverDefinitionListDialogAction);
	connect(solverDefinitionListDialogAction, SIGNAL(triggered()), m_parent->m_miscDialogManager, SLOT(solverListDialog()));

	aboutMouseAction = new QAction(tr("&Mouse Hint..."), m_helpMenu);
	aboutMouseAction->setIcon(QIcon(":/images/iconMouseHelp.png"));
	m_helpMenu->addAction(aboutMouseAction);
	connect(aboutMouseAction, SIGNAL(triggered()), m_parent->m_miscDialogManager, SLOT(mouseHelp()));

	m_helpMenu->addSeparator();

	aboutAction = new QAction(tr("&About..."), m_helpMenu);
	m_helpMenu->addAction(aboutAction);
	connect(aboutAction, SIGNAL(triggered()), m_parent->m_miscDialogManager, SLOT(about()));
}

void iRICMainWindowActionManager::updateSolverList(SolverDefinitionList* /*list*/){
/*
		QList<QAction*> actions = m_newProjectMenu->actions();
		QList<QAction*>::iterator it;
		for (it = actions.begin(); it != actions.end(); ++it){
				m_newProjectSignals->removeMappings(*it);
		}
		m_newProjectMenu->clear();
		QList<SolverDefinitionAbstract*> sdlist = list->solverList();
		QList<SolverDefinitionAbstract*>::iterator sd_it;
		for (sd_it = sdlist.begin(); sd_it != sdlist.end(); ++sd_it){
				SolverDefinitionAbstract* abst = (*sd_it);
				QAction *act = new QAction(abst->caption(), m_newProjectMenu);
				m_newProjectMenu->addAction(act);
				connect(act, SIGNAL(triggered()), m_newProjectSignals, SLOT(map()));
				m_newProjectSignals->setMapping(act, (*sd_it));
		}
 */
}

void iRICMainWindowActionManager::projectFileOpen()
{
	m_isProjectFileOpen = true;
	// enable menus available only when project file is open.
	saveAction->setEnabled(true);
	saveAsFileAction->setEnabled(true);
	saveAsProjectAction->setEnabled(true);
	propertyAction->setEnabled(true);
	saveSnapshotAction->setEnabled(true);

	// all import actions are enabled.
	m_importMenu->setEnabled(true);
	m_importMenuInFileMenu->setEnabled(true);
	if (m_rawDataImportMenu != 0){
		m_rawDataImportMenu->setEnabled(true);
	}
	importCalcCondAction->setEnabled(true);
	importCgnsAction->setEnabled(true);
	importMeasuredAction->setEnabled(true);
	importImageAction->setEnabled(true);
	importGoogleMapImageAction->setEnabled(true);

	// all export actions are enabled.
	m_exportMenu->setEnabled(true);
	enableActions(m_exportMenu->actions(), true);

	openWorkFolderAction->setEnabled(true);

	// @todo currently, run action is enabled when a project data
	// is created. but, it is not wrong: run action should be
	// enabled AFTER crating all needed input data.
	m_simulationMenu->setEnabled(true);
	solverRunAction->setEnabled(true);
	solverAboutAction->setEnabled(true);
	solverClearResultAction->setEnabled(true);

	// for edit action, actions except for undo, redo are enabled.
	enableActions(m_editMenu->actions(), true);
	undoAction->setDisabled(true);
	redoAction->setDisabled(true);

	m_resultMenu->setEnabled(true);
	enableActions(m_resultMenu->actions(), true);
	// all window related actions are enabled.
	windowFocusPreProcessorAction->setEnabled(true);
	windowFocusSolverConsoleAction->setEnabled(true);
}

void iRICMainWindowActionManager::projectFileClose()
{
	m_isProjectFileOpen = false;
	// enable menus available only when project file is open.
	saveAction->setEnabled(false);
	saveAsFileAction->setEnabled(false);
	saveAsProjectAction->setEnabled(false);
	propertyAction->setEnabled(false);
	saveSnapshotAction->setEnabled(false);
	saveContinuousSnapShotAction->setEnabled(false);

	// all import actions are excepts CGNS import action are disabled.
	m_importMenu->setEnabled(true);
	m_importMenuInFileMenu->setEnabled(true);
	if (m_rawDataImportMenu != 0){
		m_rawDataImportMenu->setEnabled(false);
	}
	importCalcCondAction->setEnabled(false);
	importCgnsAction->setEnabled(true);
	importMeasuredAction->setEnabled(false);
	importImageAction->setEnabled(false);
	importGoogleMapImageAction->setEnabled(false);

	// all export actions are enabled.
	m_exportMenu->setEnabled(false);
	enableActions(m_exportMenu->actions(), false);

	openWorkFolderAction->setEnabled(false);

	m_simulationMenu->setEnabled(false);
	solverRunAction->setEnabled(false);
	solverAboutAction->setEnabled(false);
	solverClearResultAction->setEnabled(false);

	// for edit action, actions except for undo, redo are enabled.
	enableActions(m_editMenu->actions(), false);
	undoAction->setDisabled(false);
	redoAction->setDisabled(false);

	m_resultMenu->setEnabled(false);
	enableActions(m_resultMenu->actions(), false);
	// all window related actions are enabled.
	windowFocusPreProcessorAction->setEnabled(false);
	windowFocusSolverConsoleAction->setEnabled(false);
}

void iRICMainWindowActionManager::setupMainToolBar()
{
	m_mainToolBar = new iRICToolBar(tr("Main ToolBar"), m_parent);

	connect(viewMainToolBarAction, SIGNAL(triggered(bool)), m_mainToolBar, SLOT(setVisible(bool)));
	connect(m_mainToolBar, SIGNAL(visibilityChanged(bool)), viewMainToolBarAction, SLOT(setChecked(bool)));

	m_mainToolBar->setAllowedAreas(Qt::TopToolBarArea);
	m_mainToolBar->setFloatable(false);

	m_mainToolBar->addAction(openAction);
	m_mainToolBar->addAction(saveAction);
	m_mainToolBar->addAction(saveSnapshotAction);
	m_mainToolBar->addAction(saveContinuousSnapShotAction);

	m_mainToolBar->addSeparator();
	m_mainToolBar->addAction(undoAction);
	m_mainToolBar->addAction(redoAction);

	m_mainToolBar->addSeparator();

	m_mainToolBar->addAction(cameraFitAction);

	m_mainToolBar->addSeparator();
	m_mainToolBar->addAction(cameraResetRotationAction);
	m_mainToolBar->addAction(cameraRotate90Action);

	m_mainToolBar->addSeparator();
	m_mainToolBar->addAction(cameraXYPlaneAction);
	m_mainToolBar->addAction(cameraYZPlaneAction);
	m_mainToolBar->addAction(cameraZXPlaneAction);

	m_mainToolBar->addSeparator();
	m_mainToolBar->addAction(cameraMoveLeftAction);
	m_mainToolBar->addAction(cameraMoveRightAction);
	m_mainToolBar->addAction(cameraMoveUpAction);
	m_mainToolBar->addAction(cameraMoveDownAction);

	m_mainToolBar->addSeparator();
	m_mainToolBar->addAction(cameraZoomInAction);
	m_mainToolBar->addAction(cameraZoomOutAction);

	m_mainToolBar->addSeparator();
	m_mainToolBar->addAction(cameraZoomInXAction);
	m_mainToolBar->addAction(cameraZoomOutXAction);
	m_mainToolBar->addAction(cameraZoomInYAction);
	m_mainToolBar->addAction(cameraZoomOutYAction);

	m_mainToolBar->addSeparator();
	m_mainToolBar->addAction(solverRunAction);
	m_mainToolBar->addAction(solverStopAction);

	m_mainToolBar->addSeparator();
	m_mainToolBar->addAction(windowFocusPreProcessorAction);
	m_mainToolBar->addAction(windowFocusSolverConsoleAction);
	m_mainToolBar->addAction(windowCreateNew2dPostProcessorAction);
	m_mainToolBar->addAction(windowCreateNew2dBirdEyePostProcessorAction);
	m_mainToolBar->addAction(windowCreateNew3dPostProcessorAction);
	m_mainToolBar->addAction(windowCreateNewGraph2dHybridWindowAction);
	m_mainToolBar->addAction(windowCreateNewGraph2dScatteredWindowAction);
	m_mainToolBar->addAction(windowCreateVerificationDialogAction);

	m_mainToolBar->addSeparator();
	m_mainToolBar->addAction(calcResultReloadToolBarAction);

	m_mainToolBar->addSeparator();
	m_mainToolBar->addAction(aboutMouseAction);
}

void iRICMainWindowActionManager::setProjectData(ProjectData *d)
{
	importCalcCondAction->disconnect();
	connect(importCalcCondAction, SIGNAL(triggered()), d->mainfile(), SLOT(importCalcCondition()));

	importImageAction->disconnect();
	connect(importImageAction, SIGNAL(triggered()), d->mainfile(), SLOT(addBackgroundImage()));

	exportCalcCondAction->disconnect();
	connect(exportCalcCondAction, SIGNAL(triggered()), d->mainfile(), SLOT(exportCalcCondition()));

	d->mainfile()->cgnsFileList()->disconnect();
	connect(d->mainfile()->cgnsFileList(), SIGNAL(cgnsFilesUpdated(QList<CgnsFileList::CgnsFileEntry*>)), this, SLOT(updateCgnsList(QList<CgnsFileList::CgnsFileEntry*>)));
	connect(d->mainfile()->cgnsFileList(), SIGNAL(cgnsFileSwitched(QString)), this, SLOT(updateCurrentCgns(QString)));
}

void iRICMainWindowActionManager::enableActions(const QList<QAction*>& actions, bool enable)
{
	QList<QAction*>::const_iterator it;
	for (it = actions.begin(); it != actions.end(); ++it){
		(*it)->setEnabled(enable);
	}
}

void iRICMainWindowActionManager::uncheckActions(const QList<QAction *> &actions){
	QList<QAction*>::const_iterator it;
	for (it = actions.begin(); it != actions.end(); ++it){
		(*it)->setChecked(false);
	}
}

void iRICMainWindowActionManager::handleSolverStart()
{
	solverRunAction->setDisabled(true);
	solverStopAction->setEnabled(true);
}

void iRICMainWindowActionManager::handleSolverFinish()
{
	solverStopAction->setDisabled(true);
	solverRunAction->setEnabled(true);
}

void iRICMainWindowActionManager::updateMenuBar(){
	m_menuBar->clear();
	m_menuBar->addMenu(m_fileMenu);
	m_menuBar->addMenu(m_importMenu);

	QList<QMenu*>::iterator it;
	for (it = m_additionalMenus.begin(); it != m_additionalMenus.end(); ++it){
		m_menuBar->addMenu(*it);
	}
	m_menuBar->addMenu(m_simulationMenu);
	if (m_animationMenu != 0){
		viewAnimationToolBarAction->setVisible(true);
		if (m_isPostWindowActive){
			m_menuBar->addMenu(m_animationMenu);
			m_animationToolbar->show();
			viewAnimationToolBarAction->setEnabled(true);
			saveContinuousSnapShotAction->setEnabled(true);
		}else{
			m_animationToolbar->hide();
			viewAnimationToolBarAction->setDisabled(true);
			saveContinuousSnapShotAction->setDisabled(true);
		}
	}else{
		viewAnimationToolBarAction->setVisible(false);
	}
	// tool menu is hidden, because the functions in tools will be used
	// only by solver developers.
	// we will release the function in tool menu, as a separate program.
	m_menuBar->addMenu(m_resultMenu);
	m_menuBar->addMenu(m_viewMenu);
	m_menuBar->addMenu(m_optionMenu);
	m_menuBar->addMenu(m_helpMenu);
}

void iRICMainWindowActionManager::informSubWindowChange(QWidget* subwindow)
{
	if (subwindow == 0){
		// Window out side of iRIC is focused.
		// Do nothing.
		return;
	}
	// handle additional menus.
	AdditionalMenuWindow* menuWindow = dynamic_cast<AdditionalMenuWindow*>(subwindow);
	QList<QMenu*> additionalMenus;
	QToolBar* toolBar = 0;
	if (menuWindow != 0){
		QWidget* widget = dynamic_cast<QWidget*>(subwindow);
		connect(widget, SIGNAL(additionalMenusUpdated(QList<QMenu*>)), this, SLOT(updateAdditionalMenus(QList<QMenu*>)));
		additionalMenus = menuWindow->getAdditionalMenus();
		toolBar = menuWindow->getAdditionalToolBar();
	}
	setAdditionalMenus(additionalMenus);
	if (subwindow != 0){
		// update Additional tool bar
		if (m_additionalToolBar != 0){
			m_parent->removeToolBar(m_additionalToolBar);
		}
		if (toolBar != 0){
			m_parent->addToolBar(toolBar);
			toolBar->show();
		}
		m_additionalToolBar = toolBar;
	}else{
		if (m_additionalToolBar != 0){
			m_parent->removeToolBar(m_additionalToolBar);
		}
		m_additionalToolBar = toolBar;
	}
	// investigate whether it is a post processor.
	PostProcessorWindow* postWindow = dynamic_cast<PostProcessorWindow*>(subwindow);
	m_isPostWindowActive = (subwindow != 0 && postWindow != 0);
	// update camera control menus (enable or disable).
	updateCameraConnections(subwindow);
	// update "Object Browser" menu in View menu.
	updateObjectBrowserMenu(subwindow);
	// update "Attribute Browser" menu in View menu.
	updatePropertyBrowserMenu(subwindow);
	// update misc actions.
	updateMiscActions(subwindow);
	// update menubar
	updateMenuBar();
}

void iRICMainWindowActionManager::setAnimationWidgets(QMenu* m, QToolBar* t){
	m_animationMenu = m;
	m_animationToolbar = t;
	disconnect(viewAnimationToolBarAction);
	connect(viewAnimationToolBarAction, SIGNAL(toggled(bool)), m_animationToolbar, SLOT(setVisible(bool)));
	connect(m_animationToolbar, SIGNAL(visibilityChanged(bool)), viewAnimationToolBarAction, SLOT(setChecked(bool)));
}

void iRICMainWindowActionManager::updateCameraConnections(QWidget* w)
{
	updateCameraAction(cameraFitAction, w, SLOT(cameraFit()));
	updateCameraAction(cameraResetRotationAction, w, SLOT(cameraResetRotation()));
	updateCameraAction(cameraRotate90Action, w, SLOT(cameraRotate90()));
	updateCameraAction(cameraXYPlaneAction, w, SLOT(cameraXYPlane()));
	updateCameraAction(cameraYZPlaneAction, w, SLOT(cameraYZPlane()));
	updateCameraAction(cameraZXPlaneAction, w, SLOT(cameraZXPlane()));
	updateCameraAction(cameraMoveLeftAction, w, SLOT(cameraMoveLeft()));
	updateCameraAction(cameraMoveRightAction, w, SLOT(cameraMoveRight()));
	updateCameraAction(cameraMoveUpAction, w, SLOT(cameraMoveUp()));
	updateCameraAction(cameraMoveDownAction, w, SLOT(cameraMoveDown()));
	updateCameraAction(cameraZoomInAction, w, SLOT(cameraZoomIn()));
	updateCameraAction(cameraZoomOutAction, w, SLOT(cameraZoomOut()));
	updateCameraAction(cameraZoomInXAction, w, SLOT(cameraZoomInX()));
	updateCameraAction(cameraZoomOutXAction, w, SLOT(cameraZoomOutX()));
	updateCameraAction(cameraZoomInYAction, w, SLOT(cameraZoomInY()));
	updateCameraAction(cameraZoomOutYAction, w, SLOT(cameraZoomOutY()));
}

void iRICMainWindowActionManager::updateObjectBrowserMenu(QWidget* w)
{
	WindowWithObjectBrowser* window = dynamic_cast<WindowWithObjectBrowser*>(w);
	viewObjectBrowserAction->disconnect();
	if (window == 0){
		viewObjectBrowserAction->setDisabled(true);
		viewObjectBrowserAction->setChecked(false);
	}else{
		viewObjectBrowserAction->setEnabled(true);
		ObjectBrowser* o = window->objectBrowser();
		viewObjectBrowserAction->setChecked(o->isVisible());
		connect(viewObjectBrowserAction, SIGNAL(triggered(bool)), o, SLOT(setVisible(bool)));
		connect(o, SIGNAL(visibilityChanged(bool)), viewObjectBrowserAction, SLOT(setChecked(bool)));
	}
}

void iRICMainWindowActionManager::updatePropertyBrowserMenu(QWidget* w)
{
	WindowWithPropertyBrowser* window = dynamic_cast<WindowWithPropertyBrowser*>(w);
	viewPropertyBrowserAction->disconnect();
	if (window == 0){
		viewPropertyBrowserAction->setDisabled(true);
		viewPropertyBrowserAction->setChecked(false);
	}else{
		viewPropertyBrowserAction->setEnabled(true);
		PropertyBrowser* p = window->propertyBrowser();
		viewPropertyBrowserAction->setChecked(p->isVisible());
		connect(viewPropertyBrowserAction, SIGNAL(triggered(bool)), p, SLOT(setVisible(bool)));
		connect(p, SIGNAL(visibilityChanged(bool)), viewPropertyBrowserAction, SLOT(setChecked(bool)));
	}
}

void iRICMainWindowActionManager::updateCameraAction(QAction* a, QWidget* w, const char* member)
{
	// disconnect old connections.
	a->disconnect();
	bool success = connect(a, SIGNAL(triggered()), w, member);
	a->setEnabled(success);
}

void iRICMainWindowActionManager::updateWindowList()
{
	QList<QAction*> actions = m_viewMenu->actions();
	QList<QAction*>::iterator it, it2;
	it = actions.begin();
	while ((*it) != m_windowMenuSeparetor){
		++it;
	}
	// now it is m_windowMenuSeparator. goto the first window item.
	++it;
	for (it2 = it; it2 != actions.end(); ++it2){
		m_windowActivationMapper->removeMappings(*it2);
		delete *it2;
	}
	actions.erase(it, actions.end());
	// now, build window list.
	QMdiArea* mdiArea = dynamic_cast<QMdiArea*>(m_parent->centralWidget());
	QList<QMdiSubWindow*>windowList = mdiArea->subWindowList();
	QList<QMdiSubWindow*>::iterator lit;
	int i = 1;
	for (lit = windowList.begin(); lit != windowList.end(); ++lit){
		QMdiSubWindow* w = *lit;
		QString tmp = QString("%1%2 ").append(w->windowTitle());
		QString title;
		if (i < 10){
			title = tmp.arg("&").arg(i);
		}else{
			title = tmp.arg("").arg(i);
		}
		QAction* a = new QAction(title, m_viewMenu);
		a->setEnabled(m_isProjectFileOpen);
		a->setIcon(w->windowIcon());
		m_viewMenu->addAction(a);
		connect(a, SIGNAL(triggered()), m_windowActivationMapper, SLOT(map()));
		m_windowActivationMapper->setMapping(a, w);
		++i;
	}
}

void iRICMainWindowActionManager::addGridImportMenu(QMenu* menu)
{
	PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*> (m_parent->preProcessorWindow());
	pre->addGridImportMenu(menu);
}

void iRICMainWindowActionManager::addGridExportMenu(QMenu* menu)
{
	PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*> (m_parent->preProcessorWindow());
	pre->addGridExportMenu(menu);
}

void iRICMainWindowActionManager::activateWindow(QWidget* w)
{
	w->show();
	w->setFocus();
}

void iRICMainWindowActionManager::updateMiscActions(QWidget* w)
{
	updateCameraAction(viewBackgroundColorAction, w, SLOT(editBackgroundColor()));
	updateCameraAction(viewZDirectionScaleAction, w, SLOT(editZScale()));
}

void iRICMainWindowActionManager::updateAdditionalMenus(const QList<QMenu*>& menus)
{
	m_additionalMenus = menus;
	updateMenuBar();
}
