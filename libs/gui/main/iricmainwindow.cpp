#include "../continuoussnapshot/continuoussnapshotmoviepropertypage.h"
#include "../continuoussnapshot/continuoussnapshotwizard.h"
#include "../factory/postprocessorwindowfactory.h"
#include "../factory/postprocessorwindowfactory.h"
#include "../misc/animationcontroller.h"
#include "../misc/filesystemfunction.h"
#include "../misc/flushrequester.h"
#include "../misc/iricmainwindowactionmanager.h"
#include "../misc/iricmainwindowmiscdialogmanager.h"
#include "../misc/newprojectsolverselectingdialog.h"
#include "../misc/projecttypeselectdialog.h"
#include "../misc/recentprojectsmanager.h"
#include "../misc/recentsolversmanager.h"
#include "../pref/preferencedialog.h"
#include "../projectproperty/projectpropertydialog.h"
#include "../solverdef/solverdefinitionlist.h"
#include "../startpage/startpagedialog.h"
#include "iricmainwindow.h"
#include "private/iricmainwindow_calculatedresultmanager.h"
#include "private/iricmainwindow_modelessdialogmodechanger.h"
#include "private/iricmainwindow_snapshotsaver.h"

#include <cs/coordinatesystembuilder.h>
#include <gridcreatingcondition/externalprogram/gridcreatingconditioncreatorexternalprogram.h>
#include <guibase/cursorchanger.h>
#include <guibase/executer/silentexecuterwatcher.h>
#include <guibase/irictoolbar.h>
#include <guibase/widget/itemselectingdialog.h>
#include <guicore/base/clipboardoperatablewindowinterface.h>
#include <guicore/base/windowwithtmsi.h>
#include <guicore/base/windowwithzindexinterface.h>
#include <guicore/executer/iricmainwindowexecuterwatcher.h>
#include <postbase/autoparticlewindowi.h>
#include <postbase/particleexportwindowi.h>
#include <postbase/svkmlexportwindowi.h>
#include <guicore/misc/iricmetadata.h>
#include <guicore/misc/mousepositionwidget.h>
#include <guicore/post/postprocessorwindowprojectdataitem.h>
#include <guicore/postcontainer/postdataexportdialog.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/posttimesteps.h>
#include <guicore/project/projectcgnsfile.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/project/projectpostprocessors.h>
#include <guicore/project/projectworkspace.h>
#include <guicore/solverdef/solverdefinition.h>
#include <guicore/solverdef/solverdefinitionabstract.h>
#include <misc/errormessage.h>
#include <misc/filesystemfunction.h>
#include <misc/informationdialog.h>
#include <misc/iricundostack.h>
#include <misc/iricrootpath.h>
#include <misc/lastiodirectory.h>
#include <misc/networksetting.h>
#include <misc/stringtool.h>
#include <misc/valuechangert.h>
#include <misc/xmlsupport.h>
#include <misc/ziparchive.h>
#include <postbase/cfshapeexportwindowi.h>
#include <post/graph2dhybrid/graph2dhybridwindowprojectdataitem.h>
#include <post/graph2dscattered/graph2dscatteredwindowprojectdataitem.h>
#include <post/graph2dverification/graph2dverificationwindowprojectdataitem.h>
#include <post/post2d/post2dwindow.h>
#include <pre/factory/gridcreatingconditionfactory.h>
#include <pre/factory/gridexporterfactory.h>
#include <pre/factory/gridimporterfactory.h>
#include <pre/preprocessorwindow.h>
#include <pre/preprocessorwindowprojectdataitem.h>
#include <geodata/riversurvey/geodatariversurveycrosssectionwindow.h>
#include <solverconsole/solverconsolewindow.h>
#include <solverconsole/solverconsolewindowprojectdataitem.h>

#include <vtkObject.h>

#include <QApplication>
#include <QClipboard>
#include <QCryptographicHash>
#include <QDomNode>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QNetworkProxy>
#include <QPainter>
#include <QProgressDialog>
#include <QRegExp>
#include <QSettings>
#include <QSlider>
#include <QStatusBar>
#include <QThread>
#include <QTime>
#include <QXmlStreamWriter>

#include <iriclib_errorcodes.h>

iRICMainWindow::iRICMainWindow(bool cuiMode, QWidget* parent) :
	iRICMainWindowInterface(parent),
	m_miscDialogManager {new iRICMainWindowMiscDialogManager {this}},
	m_workspace {new ProjectWorkspace {this}},
	m_projectData {nullptr},
	m_coordinateSystemBuilder {new CoordinateSystemBuilder {this}},
	m_animationController {new AnimationController {this}},
	m_isOpening {false},
	m_isSaving {false},
	m_continuousSnapshotInProgress {false},
	m_cuiMode {cuiMode},
	m_metaData {nullptr},
	m_postWindowFactory {new PostProcessorWindowFactory {this}}
{
	// setup undo stack
	iRICUndoStack::initialize(this);

	// load plugins.
	loadPlugins();

	setWindowIcon(QIcon(":/images/iconiRIC.png"));

	setupCentralWidget();
	setupStatusBar();

	parseArgs();
	initSetting();

	setupNetworkProxy();
	setupBasicSubWindows();

	QDir iricDir = QDir(iRICRootPath::get());
	iricDir.cdUp();
	iricDir.cdUp();

	// Setup solver definition list.
	m_solverDefinitionList = new SolverDefinitionList(iricDir.absoluteFilePath("solvers"), iricDir.absoluteFilePath("private/solvers"), m_locale, this);
	// Setup tool definition list.
	m_guiToolList = new SolverDefinitionList(iricDir.absoluteFilePath("guis"), m_locale, this);

	// setup action manager
	m_actionManager = new iRICMainWindowActionManager(this);
	m_actionManager->projectFileClose();
	connect(m_animationController, SIGNAL(indexChanged(uint)), this, SLOT(setCurrentStep(uint)));
	connect(this, SIGNAL(allPostProcessorsUpdated()), m_animationController, SLOT(handleRenderingEnded()));

	setMenuBar(m_actionManager->menuBar());
	addToolBar(m_actionManager->mainToolBar());
	addToolBar(Qt::LeftToolBarArea, m_actionManager->windowsToolBar());
	addToolBarBreak(Qt::TopToolBarArea);

	// Update "New" submenu using the solver definition list.
	m_actionManager->updateSolverList(m_solverDefinitionList);
	// create connections, to update solver list in "New" menu automatically.
	connect(m_solverDefinitionList, SIGNAL(updated(SolverDefinitionList*)), m_actionManager, SLOT(updateSolverList(SolverDefinitionList*)));

	// Setup grid creating program list
	m_gridCreatingConditionCreators = GridCreatingConditionCreatorExternalProgram::getList(m_locale, this);

	updatePostActionStatus();
	setupAboutDialog();
	updateWindowTitle();
	connect(m_centralWidget, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(ActiveSubwindowChanged(QMdiSubWindow*)));

	restoreWindowState();
	setupProcessEnvironment();

	statusBar()->showMessage(iRICMainWindow::tr("Ready"));
}

iRICMainWindow::~iRICMainWindow()
{
	QSettings settings;
	settings.setValue("general/lastiodir", LastIODirectory::get());

	delete m_metaData;

	saveWindowState();
}

PreProcessorWindow* iRICMainWindow::preProcessorWindow() const
{
	return m_preProcessorWindow;
}

SolverConsoleWindow* iRICMainWindow::solverConsoleWindow() const
{
	return m_solverConsoleWindow;
}

void iRICMainWindow::setupBasicSubWindows()
{
	QMdiSubWindow* w = nullptr;
	m_preProcessorWindow = new PreProcessorWindow(this);
	w = m_centralWidget->addSubWindow(m_preProcessorWindow);
	w->setWindowIcon(QIcon(":/images/iconPreprocessing.png"));
	w->hide();

	m_solverConsoleWindow = new SolverConsoleWindow(this);
	w = m_centralWidget->addSubWindow(m_solverConsoleWindow);
	w->setWindowIcon(QIcon(":/libs/solverconsole/images/iconSolver.png"));
	w->hide();
}

void iRICMainWindow::setupCentralWidget()
{
	m_centralWidget = new QMdiArea(this);
	m_centralWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	m_centralWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	m_centralWidget->setActivationOrder(QMdiArea::CreationOrder);
	setCentralWidget(m_centralWidget);
}

void iRICMainWindow::newProject()
{
	if (isSolverRunning()) {
		warnSolverRunning();
		return;
	}

	NewProjectSolverSelectingDialog dialog(m_solverDefinitionList, this);

	auto mostRecentSolver = RecentSolversManager::mostRecentSolver();
	if (! mostRecentSolver.isEmpty()) {
		dialog.setSolver(mostRecentSolver);
	}

	if (dialog.exec() == QDialog::Rejected) {return;}

	auto selectedSolver = dialog.selectedSolver();
	RecentSolversManager::append(selectedSolver->folderName());

	newProject(selectedSolver);
}

void iRICMainWindow::newProject(SolverDefinitionAbstract* solver)
{
	if (isSolverRunning()) {
		warnSolverRunning();
		return;
	}
	// close project first.
	if (! closeProject()) {return;}

	// create projectdata
	QString wFolder = ProjectData::newWorkfolderName(m_workspace->workspace());
	m_projectData = new ProjectData(wFolder, this);
	m_projectData->setVersion(m_versionNumber);
	m_projectData->mainfile()->setSolverInformation(*solver);

	// save the solver to recent solver list.
	RecentSolversManager::append(solver->folderName());

	// create solver definition data
	QString solFolder = solver->absoluteFolderName();
	try {
		SolverDefinition* def = new SolverDefinition(solFolder, m_locale);
		m_projectData->setSolverDefinition(def);
	} catch (ErrorMessage& /*message*/) {
		QMessageBox::critical(this, tr("Error"), tr("Error occured while loading Solver definition file."));
		closeProject();
		return;
	}

	if (! dynamic_cast<PreProcessorWindow*>(m_preProcessorWindow)->isSetupCorrectly()) {
		QMessageBox::critical(this, tr("Error"), tr("Error occured while loading Solver definition file."));
		closeProject();
		return;
	}

	m_mousePositionWidget->setProjectData(m_projectData);
	setupForNewProjectData();

	handleCgnsSwitch();

	bool ok = m_preProcessorWindow->setupCgnsFilesIfNeeded(true);
	if (!ok) {
		closeProject();
		return;
	}

	connect(m_projectData->mainfile()->postSolutionInfo(), SIGNAL(allPostProcessorsUpdated()), this, SIGNAL(allPostProcessorsUpdated()));
	connect(m_projectData->mainfile()->postSolutionInfo(), SIGNAL(updated()), this, SLOT(updatePostActionStatus()));
	connect(m_actionManager->openWorkFolderAction, SIGNAL(triggered()), m_projectData, SLOT(openWorkDirectory()));

	// show pre-processor window first.
	PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*>(m_preProcessorWindow);
	pre->setupDefaultGeometry();
	focusPreProcessorWindow();
	m_actionManager->informSubWindowChange(pre);
	m_solverConsoleWindow->setupDefaultGeometry();
	m_solverConsoleWindow->parentWidget()->hide();
}

void iRICMainWindow::openProject()
{
	if (isSolverRunning()) {
		warnSolverRunning();
		return;
	}
	QString fname = QFileDialog::getOpenFileName(
		this, tr("Open iRIC project file"), LastIODirectory::get(), tr("iRIC project file (*.ipro project.xml)"));
	if (fname == "") {return;}
	QFileInfo finfo(fname);
	if (finfo.fileName() == "project.xml") {
		// project folder specified.
		fname = finfo.absolutePath();
	}
	openProject(fname);
}

void iRICMainWindow::openProject(const QString& filename)
{
	if (isSolverRunning()) {
		warnSolverRunning();
		return;
	}
	// check whether the project file exists.
	if (! QFile::exists(filename)) {
		// the project file does not exists!
		QMessageBox::warning(this, tr("Warning"), tr("Project file %1 does not exists.").arg(QDir::toNativeSeparators(filename)));
		RecentProjectsManager::remove(filename);
		return;
	}

	// close project first.
	if (! closeProject()) {return;}

	CursorChanger cursorChanger(QCursor(Qt::WaitCursor), this);
	ModelessDialogModeChanger modeChanger(this);

	m_isOpening = true;
	// create projectdata
	QFileInfo fileinfo(filename);
	if (fileinfo.isDir()) {
		// Project directory is opened.
		QSettings settings;
		bool copyFolderProject = settings.value("general/copyfolderproject", true).toBool();
		if (copyFolderProject) {
			// project folder is copyed to new work folder
			QString wFolder = ProjectData::newWorkfolderName(m_workspace->workspace());
			m_projectData = new ProjectData(filename, this);
			// open project file. copying folder executed.
			qApp->processEvents();
			if (! m_projectData->copyTo(wFolder, true, true)) {
				// copying failed or canceled.
				closeProject();
				return;
			}
		} else {
			if (! iRIC::isAscii(filename)) {
				QMessageBox::warning(this, tr("Warning"), tr("Project folder path has to consist of only English characters. Please move or rename the project folder."));
				return;
			}
			QDir dir(filename);
			QFile lockFile(dir.absoluteFilePath("lock"));
			if (lockFile.exists() && ! lockFile.remove()) {
				QMessageBox::warning(this, tr("Warning"), tr("This project seems to be opened by another iRIC GUI window. You can not open a project folder from multiple GUI windows."));
				return;
			}

			m_projectData = new ProjectData(filename, this);
			m_projectData->setFilename(filename, true);

			InformationDialog::warning(
						this, tr("Warning"),
						tr("The opened project is not copied to work directory, and you'll be forced to save the modifications you make to this project. "
							 "If you want to keep the current project, please save it to another project first."), "projectfolder_notice");
		}
	} else {
		// Project file is opened.
		QString wFolder = ProjectData::newWorkfolderName(m_workspace->workspace());
		m_projectData = new ProjectData(wFolder, this);
		// open project file. unzipping executed.
		qApp->processEvents();
		if (! m_projectData->unzipFrom(filename)) {
			// unzipping failed or canceled.
			closeProject();
			return;
		}
	}
	m_projectData->setFilename(filename, fileinfo.isDir());

	try {
		m_projectData->loadSolverInformation();
	} catch (ErrorMessage& m) {
		QMessageBox::warning(this, tr("Error"), m);
		// opening project failed.
		closeProject();
		return;
	}
	// make sure whether supporting solver exists.
	QString folder = m_solverDefinitionList->supportingSolverFolder(m_projectData, this);
	if (folder.isNull()){
		m_projectData->setPostOnlyMode();
		folder = ":/data/unknownsolver";
	}
	SolverDefinition* def = new SolverDefinition(folder, m_locale);
	m_projectData->setSolverDefinition(def);

	try {
		setCursor(Qt::WaitCursor);
		m_projectData->mainfile()->load();
	} catch (ErrorMessage& m) {
		QMessageBox::warning(this, tr("Error"), m);
		closeProject();
		return;
	}

	m_projectData->setVersion(m_versionNumber);
	setupForNewProjectData();

	m_projectData->mainfile()->loadFromCgnsFile();
	handleCgnsSwitch();

	bool ok = m_preProcessorWindow->setupCgnsFilesIfNeeded(true);
	if (! ok) {
		closeProject();
		return;
	}

	connect(m_projectData->mainfile()->postSolutionInfo(), SIGNAL(allPostProcessorsUpdated()), this, SIGNAL(allPostProcessorsUpdated()));
	connect(m_projectData->mainfile()->postSolutionInfo(), SIGNAL(updated()), this, SLOT(updatePostActionStatus()));
	connect(m_actionManager->openWorkFolderAction, SIGNAL(triggered()), m_projectData, SLOT(openWorkDirectory()));

	if (m_projectData->isPostOnlyMode()) {
		// hide pre-processor.
		m_preProcessorWindow->parentWidget()->hide();
	}

	// hide condole window.
	m_solverConsoleWindow->setupDefaultGeometry();
	m_solverConsoleWindow->parentWidget()->hide();

	if (! m_projectData->isPostOnlyMode()) {
		// show preprocessor window.
		focusPreProcessorWindow();
	}
	// open post-processors.
	m_projectData->openPostProcessors();
	QList<QMdiSubWindow*> wlist = m_centralWidget->subWindowList();
	QList<QMdiSubWindow*>::iterator it;
	for (QMdiSubWindow* subw : wlist) {
		if (dynamic_cast<PostProcessorWindow*> (subw->widget()) == nullptr) {continue;}
		connect(subw, SIGNAL(destroyed(QObject*)), m_actionManager, SLOT(updateWindowList()));
	}

	iRICUndoStack::instance().clear();

	m_preProcessorWindow->setupCgnsFilesIfNeeded(false);

	m_isOpening = false;
	LastIODirectory::set(QFileInfo(filename).absolutePath());
	m_projectData->mainfile()->clearModified();
	m_mousePositionWidget->setProjectData(m_projectData);

	RecentProjectsManager::append(filename);
	updatePostActionStatus();
}

void iRICMainWindow::importCalcCondition()
{
	PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*> (m_preProcessorWindow);
	pre->importCalcCondition();
}

void iRICMainWindow::exportCalcCondition()
{
	PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*> (m_preProcessorWindow);
	pre->exportCalcCondition();
}

void iRICMainWindow::importCalculationResult()
{
	QString fname = QFileDialog::getOpenFileName(
		this, tr("Open Calculation result"), LastIODirectory::get(), tr("CGNS file (*.cgn *.cgns)")
		);
	if (fname == "") {return;}
	importCalculationResult(fname);
}

void iRICMainWindow::importCalculationResult(const QString& fname)
{
	// check whether the CGNS file exists.
	if (! QFile::exists(fname)) {
		// the CGNS file does not exists!
		QMessageBox::warning(this, tr("Warning"), tr("CGNS file %1 does not exists.").arg(QDir::toNativeSeparators(fname)));
		return;
	}
	// close project first.
	if (! closeProject()) {return;}

	m_isOpening = true;
	// create projectdata
	QString wFolder = ProjectData::newWorkfolderName(m_workspace->workspace());
	m_projectData = new ProjectData(wFolder, this);

	// load solver information from CGNS file.
	std::string solverName;
	VersionNumber versionNumber;
	bool bret = ProjectCgnsFile::readSolverInfo(fname, &solverName, &versionNumber);

	QString solFolder;
	if (bret == false) {
		m_projectData->setPostOnlyMode();
		solFolder = ":/data/unknownsolver";
	} else {
		// loading succeeded.
		m_projectData->mainfile()->setSolverName(solverName);
		m_projectData->mainfile()->setSolverVersion(versionNumber);

		// make sure whether supporting solver exists.
		QString folder = m_solverDefinitionList->supportingSolverFolder(m_projectData, this);
		if (folder.isNull()){
			m_projectData->setPostOnlyMode();
			solFolder = ":/data/unknownsolver";
		} else {
			// create solver definition data
			solFolder = folder;
		}
	}
	SolverDefinition* def = new SolverDefinition(solFolder, m_locale);
	m_projectData->setSolverDefinition(def);
	m_projectData->setVersion(m_versionNumber);

	setupForNewProjectData();

	bret = m_projectData->mainfile()->importCgnsFile(fname, "Case1");
	if (! bret) {return;}

	handleCgnsSwitch();

	connect(m_projectData->mainfile()->postSolutionInfo(), SIGNAL(allPostProcessorsUpdated()), this, SIGNAL(allPostProcessorsUpdated()));
	connect(m_projectData->mainfile()->postSolutionInfo(), SIGNAL(updated()), this, SLOT(updatePostActionStatus()));
	connect(m_actionManager->openWorkFolderAction, SIGNAL(triggered()), m_projectData, SLOT(openWorkDirectory()));

	// show pre-processor window first.
	if (! m_projectData->isPostOnlyMode()) {
		// show pre-processor window first.
		auto pre = dynamic_cast<PreProcessorWindow*> (m_preProcessorWindow);
		pre->setupDefaultGeometry();
		pre->parentWidget()->show();
		m_actionManager->informSubWindowChange(m_preProcessorWindow);
		m_solverConsoleWindow->setupDefaultGeometry();
		m_solverConsoleWindow->parentWidget()->hide();
	}

	m_isOpening = false;
	LastIODirectory::set(QFileInfo(fname).absolutePath());
	m_projectData->mainfile()->setModified();

	// update recently opened projects.
	updatePostActionStatus();
}

const QProcessEnvironment& iRICMainWindow::processEnvironment() const
{
	return m_processEnvironment;
}

void iRICMainWindow::updateCrosssectionWindows()
{
	for (QMdiSubWindow* w : m_centralWidget->subWindowList()) {
		auto widget = w->widget();
		auto csw = dynamic_cast<GeoDataRiverSurveyCrosssectionWindow*> (widget);
		if (csw == nullptr) {continue;}

		csw->updateRiverSurveys();
	}
}

ExecuterWatcher* iRICMainWindow::buildExecuteWatcher(ExecuterI* executer)
{
	if (m_cuiMode) {
		return new SilentExecuterWatcher(executer);
	} else {
		return new iRICMainWindowExecuterWatcher(executer, this);
	}
}

bool iRICMainWindow::cuiMode() const
{
	return m_cuiMode;
}

void iRICMainWindow::reloadCgnsFile()
{
	if (isSolverRunning()) {
		warnSolverRunning();
		return;
	}

	m_projectData->mainfile()->closeCgnsFile();
	m_projectData->mainfile()->loadFromCgnsFile();
}

bool iRICMainWindow::closeProject()
{
	if (m_projectData == nullptr) {return true;}
	bool result = true;
	if (! m_projectData->isPostOnlyMode() && m_projectData->mainfile()->isModified()) {
		if (! m_projectData->isInWorkspace()) {
			// always save.
			result = saveProject();
		} else {
			QMessageBox::StandardButton button = QMessageBox::warning(
				this,
				tr("Warning"),
				tr("This Project is modified. Do you want to save?"),
				QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
				QMessageBox::Cancel
					);
			switch (button) {
			case QMessageBox::Yes:
				// save data.
				result = saveProject();
				break;
			case QMessageBox::No:
				// not needed to save.
				result = true;
				break;
			case QMessageBox::Cancel:
				result = false;
				break;
			default:
				break;
			}
		}
	}
	if (! result) {
		return false;
	}

	m_actionManager->unregisterAdditionalToolBar();
	addToolBarBreak(Qt::TopToolBarArea);
	m_actionManager->projectFileClose();

	auto preParent = m_preProcessorWindow->parentWidget();
	preParent->showNormal();
	preParent->hide();

	auto consoleParent = m_solverConsoleWindow->parentWidget();
	consoleParent->showNormal();
	consoleParent->hide();

	m_postWindowFactory->resetWindowCounts();
	ActiveSubwindowChanged(dynamic_cast<QMdiSubWindow*>(m_solverConsoleWindow->parentWidget()));
	delete m_projectData;
	m_projectData = nullptr;
	m_mousePositionWidget->setProjectData(0);
	updateWindowTitle();
	updatePostActionStatus();

	iRICUndoStack& stack = iRICUndoStack::instance();
	stack.clear();
	stack.setClean();
	return true;
}

void iRICMainWindow::setupForNewProjectData()
{
	// Inform that project file is opened.
	m_actionManager->projectFileOpen();
	if (m_projectData->isPostOnlyMode()) {
		m_actionManager->setMode(iRICMainWindowActionManager::Mode::PostOnly);
	}
	// Update post-processor related action status.
	updatePostActionStatus();

	// update animationcontroller
	auto ac = m_animationController;
	ac->setup(m_projectData->solverDefinition()->iterationType());
	QToolBar* at = ac->animationToolBar();
	if (at != nullptr) {addToolBar(at);}

	m_actionManager->setAnimationWidgets(ac->animationMenu(), at);
	m_actionManager->updateMenuBar();

	// update window title.
	updateWindowTitle();
}

void iRICMainWindow::ActiveSubwindowChanged(QMdiSubWindow* newActiveWindow)
{
	if (m_projectData == nullptr) {
		// project is not open.
		return;
	}
	m_actionManager->updateWindowList();
	if (newActiveWindow == nullptr) {
		// Window of other program is activated.
		m_actionManager->informSubWindowChange(nullptr);
		return;
	}
	QWidget* innerWindow = newActiveWindow->widget();
	GeoDataRiverSurveyCrosssectionWindow* cw = dynamic_cast<GeoDataRiverSurveyCrosssectionWindow*>(innerWindow);
	if (cw != nullptr) {
		cw->informFocusIn();
	} else {
		PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*>(m_preProcessorWindow);
		pre->informUnfocusRiverCrosssectionWindows();
	}
	m_mousePositionWidget->clear();
	connect(innerWindow, SIGNAL(worldPositionChangedForStatusBar(QPointF)), m_mousePositionWidget, SLOT(updatePosition(QPointF)));
	m_actionManager->informSubWindowChange(innerWindow);
}

void iRICMainWindow::focusPreProcessorWindow()
{
	QWidget* parent = m_preProcessorWindow->parentWidget();
	parent->show();
	parent->setFocus();
	ActiveSubwindowChanged(dynamic_cast<QMdiSubWindow*>(parent));
}

void iRICMainWindow::focusSolverConsoleWindow()
{
	QWidget* parent = m_solverConsoleWindow->parentWidget();
	parent->show();
	parent->setFocus();
	ActiveSubwindowChanged(dynamic_cast<QMdiSubWindow*>(parent));
}

bool iRICMainWindow::saveProjectAsFile()
{
	if (isSolverRunning()) {
		warnSolverRunning();
		return false;
	}
	QString fname = QFileDialog::getSaveFileName(
		this, tr("Save iRIC project file"), LastIODirectory::get(), tr("iRIC project file (*.ipro)"));
	if (fname == "") {return false;}
	return saveProject(fname, false);
}

bool iRICMainWindow::saveProjectAsFolder()
{
	if (isSolverRunning()) {
		warnSolverRunning();
		return false;
	}
INPUTFOLDERNAME:
	QString foldername = QFileDialog::getExistingDirectory(this, tr("Save iRIC project"), LastIODirectory::get());
	if (foldername == "") {return false;}
	if (! iRIC::isAscii(foldername)) {
		QMessageBox::critical(this, tr("Error"), tr("Project folder path has to consist of only English characters."));
		goto INPUTFOLDERNAME;
	}
	if (! iRIC::isDirEmpty(foldername)) {
		QMessageBox::critical(this, tr("Error"), tr("The project folder has to be empty."));
		goto INPUTFOLDERNAME;
	}
	return saveProject(foldername, true);
}

bool iRICMainWindow::saveProject()
{
	if (isSolverRunning()) {
		warnSolverRunning();
		return false;
	}

	if (m_projectData->filename() == "") {
		// select how to save: project file or folder.
		ProjectTypeSelectDialog dialog(this);
		int ret = dialog.exec();
		if (ret == QDialog::Rejected) {return false;}
		if (dialog.folderProject()) {
			return saveProjectAsFolder();
		} else {
			return saveProjectAsFile();
		}
	} else {
		return saveProject(m_projectData->filename(), m_projectData->folderProject());
	}
}

bool iRICMainWindow::saveProject(const QString& filename, bool folder)
{
	ValueChangerT<bool> savingChanger(&m_isSaving, true);
	CursorChanger cursorChanger(QCursor(Qt::WaitCursor), this);
	ModelessDialogModeChanger modeChanger(this);

	bool ret;
	auto mainfile = m_projectData->mainfile();
	if (m_projectData->isPostOnlyMode()) {
		// do not save CGNS file, but only projext.xml.
		ret = mainfile->saveExceptCGNS();
	} else {
		auto pre = dynamic_cast<PreProcessorWindow*>(m_preProcessorWindow);
		auto gridEdited = pre->projectDataItem()->isGridEdited();
		auto hasResult = m_projectData->mainfile()->postSolutionInfo()->hasResults();

		if (gridEdited && hasResult) {
			int ret = QMessageBox::warning(m_preProcessorWindow, tr("Warning"), tr("The grids are edited or deleted. When you save, the calculation result is discarded."), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
			if (ret == QMessageBox::Cancel) {return false;}
		}

		if (gridEdited || (! hasResult)) {
			int ier = mainfile->saveToCgnsFile();
			ret = (ier == IRIC_NO_ERROR);
		} else {
			int ier = mainfile->updateCgnsFileOtherThanGrids();
			ret = (ier == IRIC_NO_ERROR);
		}
		if (ret) {ret = mainfile->saveExceptCGNS();}
		if (ret) {mainfile->setModified(false);}
	}

	if (! ret) {
		QMessageBox::critical(this, tr("Error"), tr("Saving project failed."));
		return false;
	}

	QSettings settings;
	bool copyFolderProject = settings.value("general/copyfolderproject", true).toBool();

	if (folder) {
		if (! m_projectData->isInWorkspace()) {
			// working on the project folder.
			if (m_projectData->filename() == filename) {
				// do nothing.
			} else {
				// copy project.
				ret = m_projectData->copyTo(filename, true);
			}
		} else {
			// working on the working folder.
			if (copyFolderProject) {
				ret = m_projectData->copyTo(filename, false);
			} else {
				ret = m_projectData->moveTo(filename);
			}
		}
	} else {
#ifdef USE_32BIT_ZIP_UNZIP
		if (m_projectData->hasHugeCgns()) {
			QMessageBox::critical(this, tr("Error"), tr("This project has HUGE calculation result, so it cannot be saved as a file (*.ipro). Please save as a project."));
			return false;
		}
		if (m_projectData->hasTooManyInnerFiles()) {
			QMessageBox::critical(this, tr("Error"), tr("This project has too many files, so it cannot be saved as a file (*.ipro). Please save as a project."));
			return false;
		}
#endif
		if (! m_projectData->isInWorkspace()) {
			// working on the project folder.
			QString newWorkFolder = ProjectData::newWorkfolderName(m_workspace->workspace());
			ret = m_projectData->copyTo(newWorkFolder, true);
			if (ret) {
				ret = m_projectData->zipTo(filename);
			}
		} else {
			// working on the working folder.
			ret = m_projectData->zipTo(filename);
		}
	}
	if (ret) {
		m_projectData->setFilename(filename, folder);
	}

	cursorChanger.restoreCursor();
	if (! ret) {
		QMessageBox::critical(this, tr("Error"), tr("Saving project failed."));
		return false;
	}
	updateWindowTitle();
	LastIODirectory::set(QFileInfo(filename).absolutePath());
	RecentProjectsManager::append(filename);
	statusBar()->showMessage(tr("Project successfully saved to %1.").arg(QDir::toNativeSeparators(filename)), STATUSBAR_DISPLAYTIME);
	return true;
}

void iRICMainWindow::closeEvent(QCloseEvent* event)
{
	if (isSolverRunning()) {
		QMessageBox::StandardButton button = QMessageBox::warning(
			this, tr("Warning"),
			tr("The solver is still running. Really quit?"),
			QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
		if (button == QMessageBox::No) {
			event->ignore();
			return;
		}
		m_solverConsoleWindow->terminateSolver();
	}
	if (m_isSaving) {
		event->ignore();
		return;
	}
	if (! closeProject()) {
		event->ignore();
		return;
	}
	event->accept();
}

void iRICMainWindow::showProjectPropertyDialog()
{
	ProjectPropertyDialog dialog(this);
	dialog.setProjectData(m_projectData);
	dialog.exec();

	m_animationController->updateLabelAndPostWindows();
}

void iRICMainWindow::cut()
{
	// currently, do nothing.
}

void iRICMainWindow::copy()
{
	QWidget* widget = m_centralWidget->activeSubWindow()->widget();
	if (widget == m_solverConsoleWindow) {
		dynamic_cast<SolverConsoleWindow*>(widget)->copy();
	}
}

void iRICMainWindow::paste()
{
	// currently, do nothing.
}

void iRICMainWindow::snapshot()
{
	QWidget* widget = m_centralWidget->activeSubWindow()->widget();
	SnapshotEnabledWindowInterface* enabledWindow = dynamic_cast<SnapshotEnabledWindowInterface*>(widget);

	if (enabledWindow == nullptr) {
		QMessageBox::warning(this, tr("Warning"), tr("This windows does not support snapshot function."));
		return;
	}

	SnapshotSaver saver(this);
	saver.save(enabledWindow);
}

void iRICMainWindow::copySnapshot()
{
	QWidget* widget = m_centralWidget->activeSubWindow()->widget();
	SnapshotEnabledWindowInterface* enabledWindow = dynamic_cast<SnapshotEnabledWindowInterface*>(widget);

	if (enabledWindow == nullptr) {
		QMessageBox::warning(this, tr("Warning"), tr("This windows does not support snapshot function."));
		return;
	}

	enabledWindow->setTransparent(false);
	QPixmap pixmap = enabledWindow->snapshot();

	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setPixmap(pixmap);
	statusBar()->showMessage(tr("Copied snapshot to clipboard."), STATUSBAR_DISPLAYTIME);
}

void iRICMainWindow::snapshotSvg()
{}

void iRICMainWindow::continuousSnapshot()
{
	if (m_solverConsoleWindow->isSolverRunning()) {
		QMessageBox::warning(this, tr("Warning"), tr("This menu is not available while the solver is running."), QMessageBox::Ok);
		return;
	}
	QWidget* widget = m_centralWidget->activeSubWindow()->widget();
	SnapshotEnabledWindowInterface* enableWindow = dynamic_cast<SnapshotEnabledWindowInterface*>(widget);
	if (enableWindow != nullptr) {
		ContinuousSnapshotWizard* wizard = new ContinuousSnapshotWizard(this);

		wizard->setSetting(m_continuousSnapshotSetting);

		wizard->setProjectMainFile(m_projectData->mainfile());
		wizard->setTargetWindow(0);

		if (wizard->exec() == QDialog::Accepted) {
			m_continuousSnapshotSetting = wizard->setting();

			const auto& s = m_continuousSnapshotSetting;
			if (s.outputKml) {
				// opne kml file
				QString kml = QDir(s.exportTargetFolder).absoluteFilePath(s.kmlFilename);
				QFile f(kml);
				f.open(QFile::WriteOnly);
				QXmlStreamWriter writer(&f);
				writer.setAutoFormatting(false);
				writer.writeStartDocument("1.0");
				writer.writeStartElement("kml");
				writer.writeAttribute("xmlns", "http://earth.google.com/kml/2.1");
				writer.writeStartElement("Document");
				writer.writeStartElement("visibility");
				writer.writeCharacters("1");
				writer.writeEndElement();
				// save continuous snapshot
				saveContinuousSnapshot(wizard, &writer);
				// close kml file
				writer.writeEndElement();
				writer.writeEndElement();
				writer.writeEndDocument();
				f.close();
			} else {
				saveContinuousSnapshot(wizard);
			}
		}
		delete wizard;
	} else {
		QMessageBox::warning(this, tr("Warning"), tr("This windows does not support continuous snapshot function."));
	}
}

void iRICMainWindow::saveContinuousSnapshot(ContinuousSnapshotWizard* wizard, QXmlStreamWriter* writer)
{
	const auto& setting = m_continuousSnapshotSetting;

	// clear the folder to output files
	auto dir = setting.exportTargetFolder;

	if (! iRIC::isDirEmpty(dir)) {
		int ret = QMessageBox::warning(wizard, tr("Warning"), tr("All files in %1 is deleted.").arg(QDir::toNativeSeparators(dir)), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
		if (ret == QMessageBox::Cancel) {return;}

		iRIC::rmdirRecursively(dir);
	}

	iRIC::mkdirRecursively(dir);

	bool isAutoParticleOutput = false;
	for (auto sub : wizard->windowList())
	{
		auto w = dynamic_cast<AutoParticleWindowI*> (sub->widget());
		if (w == nullptr) {continue;}
		isAutoParticleOutput = isAutoParticleOutput || w->isAutoParticleOutput();
	}

	ValueChangerT<bool> snapShotInProgressChanger(&m_continuousSnapshotInProgress, true);
	QProgressDialog dialog(wizard);
	dialog.setRange(0, setting.stopTimeStep - setting.startTimeStep);
	dialog.setWindowTitle(tr("Continuous Snapshot"));
	dialog.setLabelText(tr("saving continuous snapshot..."));
	dialog.setFixedSize(300, 100);
	dialog.setModal(true);
	dialog.show();

	int step = setting.startTimeStep;
	auto fit = wizard->fileList().begin();
	QPoint position = wizard->beginPosition();
	QSize size = wizard->snapshotSize();
	QList<QSize> sizes;
	bool first = true;
	int imgCount = 0;
	m_animationController->setCurrentStepIndex(step);
	while (step <= setting.stopTimeStep) {
		dialog.setValue(step - setting.startTimeStep);
		qApp->processEvents();
		if (dialog.wasCanceled()) {
			return;
		}
		if (isAutoParticleOutput) {
			// increment step by 1, to generate particles
			while (m_animationController->currentStepIndex() != step) {
				m_animationController->setCurrentStepIndex(m_animationController->currentStepIndex() + 1);
			}
		} else {
			// skip directry to step
			m_animationController->setCurrentStepIndex(step);
		}

		switch (setting.fileOutputSetting) {
		case ContinuousSnapshotSetting::FileOutputSetting::Onefile: {
				if (first) {sizes.append(size);}
				QImage image = QImage(size, QImage::Format_ARGB32);
				QPixmap pixmap;
				QPainter painter;
				if (setting.imageIsTransparent) {
					image.fill(qRgba(0, 0, 0, 0));
					pixmap = QPixmap::fromImage(image);
					painter.begin(&pixmap);
					painter.setBackgroundMode(Qt::TransparentMode);
				} else {
					image.fill(qRgba(255, 255, 255, 255));
					pixmap = QPixmap::fromImage(image);
					painter.begin(&pixmap);
					painter.setBackgroundMode(Qt::OpaqueMode);
				}
				QPoint begin(0, 0);
				for (QMdiSubWindow* sub : wizard->windowList()) {
					SnapshotEnabledWindowInterface* window = dynamic_cast<SnapshotEnabledWindowInterface*>(sub->widget());
					QWidget* center = dynamic_cast<QMainWindow*>(sub->widget())->centralWidget();
					window->setTransparent(setting.imageIsTransparent);

					switch (setting.outputLayout) {
					case ContinuousSnapshotSetting::OutputLayout::AsIs:
						begin = sub->pos() + sub->widget()->pos() + center->pos();
						painter.drawPixmap(begin.x() - position.x(), begin.y() - position.y(), window->snapshot());
						break;

					case ContinuousSnapshotSetting::OutputLayout::Horizontally:
						painter.drawPixmap(begin.x(), begin.y(), window->snapshot());
						begin.setX(begin.x() + center->width());
						break;

					case ContinuousSnapshotSetting::OutputLayout::Vertically:
						painter.drawPixmap(begin.x(), begin.y(), window->snapshot());
						begin.setY(begin.y() + center->height());
						break;
					}
				}
				pixmap.save(*fit);
				QString url = QFileInfo(*fit).fileName();
				const auto& s = m_continuousSnapshotSetting;
				addKMLElement(step, url,  s.north, s.south, s.west, s.east, s.angle, writer);
				++fit;
				break;
			}
		case ContinuousSnapshotSetting::FileOutputSetting::Respectively:
			int i = 0;
			for (QMdiSubWindow* sub : wizard->windowList()) {
				SnapshotEnabledWindowInterface* window = dynamic_cast<SnapshotEnabledWindowInterface*>(sub->widget());
				window->setTransparent(setting.imageIsTransparent);
				QPixmap pixmap = window->snapshot();
				pixmap.save(*fit);
				if (first) {sizes.append(pixmap.size());}
				if (wizard->targetWindow() == i) {
					QString url = QFileInfo(*fit).fileName();
					const auto& s = m_continuousSnapshotSetting;
					addKMLElement(step, url, s.north, s.south, s.west, s.east, s.angle, writer);
				}
				++fit;
				++ i;
			}
			break;
		}
		step += setting.samplingRate;
		first = false;
		++ imgCount;
	}
	// output Movie.
	if (setting.outputMovie) {
		QStringList inputFilenames;
		QStringList outputFilenames;
		QString inputFilename;
		QString outputFilename;

		auto setting = wizard->setting();

		switch (setting.fileOutputSetting) {
		case ContinuousSnapshotSetting::FileOutputSetting::Onefile:
			inputFilename = QString("img_%%1d%2").arg(setting.suffixLength).arg(setting.imageExtention);
			outputFilename = QString("img.mp4");
			inputFilenames.append(inputFilename);
			outputFilenames.append(outputFilename);
			break;

		case ContinuousSnapshotSetting::FileOutputSetting::Respectively:
			int idx = 0;
			for (auto it = wizard->windowList().begin(); it != wizard->windowList().end(); ++it) {
				inputFilename = QString("window%1_%%2d%3").arg(idx + 1).arg(setting.suffixLength).arg(setting.imageExtention);
				outputFilename = QString("window%1.mp4").arg(idx + 1);
				inputFilenames.append(inputFilename);
				outputFilenames.append(outputFilename);
				++idx;
			}
			break;
		}
		QStringList profileString = ContinuousSnapshotMoviePropertyPage::getProfile(setting.movieProfile);
		for (int i = 0; i < inputFilenames.count(); ++i) {
			QString inFile  = inputFilenames.at(i);
			QString outFile = outputFilenames.at(i);
			QString absOutFile = QDir(setting.exportTargetFolder).absoluteFilePath(outFile);
			if (QFile::exists(absOutFile)) {
				bool ok = QFile::remove(absOutFile);
				if (! ok) {
					QMessageBox::warning(this, tr("Warning"), tr("%1 already exists, and failed to remove it. Movie file is not output.").arg(QDir::toNativeSeparators(absOutFile)));
					continue;
				}
			}
			QString exepath = QCoreApplication::instance()->applicationDirPath();
			QString ffmpegexe = QDir(exepath).absoluteFilePath("ffmpeg.exe");
			QProcess* ffprocess = new QProcess();
			ffprocess->setWorkingDirectory(setting.exportTargetFolder);
			QStringList args;
			if (setting.movieLengthMode == ContinuousSnapshotSetting::MovieLengthMode::Length) {
				// specify length
				args << "-r" << QString("%1/%2").arg(imgCount).arg(setting.movieLengthSeconds);
			} else {
				// specify fps
				args << "-r" << QString("%1").arg(setting.movieFramesPerSeconds);
			}
			args << "-i" << inFile;
			args << profileString;
			QSize size = sizes.at(i);
			args << "-vf" << QString("scale=%1:%2").arg((size.width() / 2) * 2).arg((size.height() / 2) * 2);
			args << outFile;
			ffprocess->start(ffmpegexe, args);
			ffprocess->waitForFinished(-1);
		}
	}
}

void iRICMainWindow::addKMLElement(int time, QString url, double north, double south, double west, double east, double angle, QXmlStreamWriter* writer)
{
	if (! writer) { return; }

	writer->writeStartElement("GroundOverlay");

	writer->writeStartElement("name");
	writer->writeCharacters(QString::number(time));
	writer->writeEndElement();

	writer->writeStartElement("visibility");
	writer->writeCharacters("1");
	writer->writeEndElement();

	writer->writeStartElement("TimeStamp");
	writer->writeStartElement("when");
	writer->writeCharacters(timeString(time));
	writer->writeEndElement();
	writer->writeEndElement();

	writer->writeStartElement("Icon");
	writer->writeStartElement("href");
	writer->writeCharacters(url);
	writer->writeEndElement();
	writer->writeStartElement("viewBoundScale");
	writer->writeCharacters("0.75");
	writer->writeEndElement();
	writer->writeEndElement();

	writer->writeStartElement("LatLonBox");
	writer->writeStartElement("north");
	writer->writeCharacters(QString::number(north, 'g', 10));
	writer->writeEndElement();
	writer->writeStartElement("south");
	writer->writeCharacters(QString::number(south, 'g', 10));
	writer->writeEndElement();
	writer->writeStartElement("east");
	writer->writeCharacters(QString::number(east, 'g', 10));
	writer->writeEndElement();
	writer->writeStartElement("west");
	writer->writeCharacters(QString::number(west, 'g', 10));
	writer->writeEndElement();
	writer->writeStartElement("rotation");
	writer->writeCharacters(QString::number(angle));
	writer->writeEndElement();
	writer->writeEndElement();

	writer->writeEndElement();
}

QString iRICMainWindow::timeString(int time)
{
	QDateTime datetime(QDate(2011, 1, 1));
	auto zeroDateTime = projectData()->mainfile()->zeroDateTime();
	if (! zeroDateTime.isNull()) {
		datetime = zeroDateTime;
	}
	auto secs = m_projectData->mainfile()->postSolutionInfo()->timeSteps()->timesteps().at(time);
	datetime = datetime.addSecs(static_cast<int>(secs));

	return datetime.toString("yyyy-MM-ddTHH:mm:ssZ");
}

void iRICMainWindow::updateWindowTitle()
{
	QString fname = "";
	if (m_projectData == nullptr) {
		setWindowTitle(tr("%1 %2").arg(m_metaData->caption()).arg(m_versionNumber.toString()));
		return;
	}
	if (m_projectData->filename() == "") {
		// Not named yet.
		fname = tr("Untitled");
	} else {
		QFileInfo finfo(m_projectData->filename());
		if (m_projectData->folderProject()) {
			fname = QDir::toNativeSeparators(finfo.absoluteFilePath());
		} else {
			fname = finfo.fileName();
		}
	}
	QString solverName = m_projectData->solverDefinition()->caption();
	QString title = tr("%1 - %2 %3 [%4]");
	setWindowTitle(title.arg(QDir::toNativeSeparators(fname)).arg(m_metaData->caption()).arg(m_versionNumber.toString()).arg(solverName));
}

void iRICMainWindow::warnSolverRunning() const
{
	iRICMainWindow* t = const_cast<iRICMainWindow*> (this);
	QMessageBox::warning(t, tr("Warning"), tr("The solver is running now. Please stop solver, to do this action."), QMessageBox::Ok);
}

const QLocale iRICMainWindow::locale() const
{
	return m_locale;
}

bool iRICMainWindow::isSolverRunning() const
{
	return m_solverConsoleWindow->isSolverRunning();
}

ProjectWorkspace* iRICMainWindow::workspace()
{
	return m_workspace;
}

const VersionNumber& iRICMainWindow::versionNumber() const
{
	return m_versionNumber;
}

void iRICMainWindow::setupAnimationToolbar()
{
	connect(m_projectData->mainfile()->postSolutionInfo(), SIGNAL(cgnsTimeStepsUpdated(QList<double>)), m_animationController, SLOT(updateTimeSteps(QList<double>)));
	connect(m_projectData->mainfile()->postSolutionInfo(), SIGNAL(cgnsIterationStepsUpdated(QList<int>)), m_animationController, SLOT(updateIterationSteps(QList<int>)));

	m_projectData->mainfile()->postSolutionInfo()->informCgnsSteps();
}

void iRICMainWindow::handleCgnsSwitch()
{
	setupAnimationToolbar();

	// clear solver console window.
	if (! m_isOpening) {
		m_solverConsoleWindow->clear();
	}

	// create a connection so that when the solver finishes
	// the calculation, the the cgns file automatically
	// checks wheter the steps stored in the CGNS file
	// is updated.
	connect(m_solverConsoleWindow, SIGNAL(solverStarted()), m_projectData->mainfile()->postSolutionInfo(), SLOT(informSolverStart()));
	connect(m_solverConsoleWindow, SIGNAL(solverFinished()), m_projectData->mainfile()->postSolutionInfo(), SLOT(checkCgnsStepsUpdate()));
	connect(m_solverConsoleWindow, SIGNAL(solverFinished()), m_projectData->mainfile()->postSolutionInfo(), SLOT(informSolverFinish()));
}

void iRICMainWindow::exportCurrentCgnsFile()
{
	if (isSolverRunning()) {
		warnSolverRunning();
		return;
	}
	// export the current CGNS file.
	m_projectData->mainfile()->exportCurrentCgnsFile();
}

void iRICMainWindow::setCurrentStep(unsigned int newstep)
{
	if (m_projectData != nullptr) {
		m_projectData->mainfile()->postSolutionInfo()->setCurrentStep(newstep);
	}
}

void iRICMainWindow::create2dPostWindow()
{
	static int index = 1;
	if (index == 10) {
		index = 1;
	}
	ProjectPostProcessors* posts = m_projectData->mainfile()->postProcessors();
	PostProcessorWindowProjectDataItem* item = m_postWindowFactory->factory("post2dwindow", posts, this);
	QMdiSubWindow* container = posts->add(item);
	container->show();
	container->setFocus();
	item->window()->setupDefaultGeometry(index);
	++index;
	connect(container, SIGNAL(destroyed(QObject*)), m_actionManager, SLOT(updateWindowList()));
}

void iRICMainWindow::create2dBirdEyePostWindow()
{
	static int index = 1;
	if (index == 10) {
		index = 1;
	}
	ProjectPostProcessors* posts = m_projectData->mainfile()->postProcessors();
	PostProcessorWindowProjectDataItem* item = m_postWindowFactory->factory("post2dbirdeyewindow", posts, this);
	QMdiSubWindow* container = posts->add(item);
	container->show();
	container->setFocus();
	item->window()->setupDefaultGeometry(index);
	++index;
	connect(container, SIGNAL(destroyed(QObject*)), m_actionManager, SLOT(updateWindowList()));
}

void iRICMainWindow::create3dPostWindow()
{
	static int index = 1;
	if (index == 10) {
		index = 1;
	}
	ProjectPostProcessors* posts = m_projectData->mainfile()->postProcessors();
	PostProcessorWindowProjectDataItem* item = m_postWindowFactory->factory("post3dwindow", posts, this);
	QMdiSubWindow* container = posts->add(item);
	container->show();
	container->setFocus();
	item->window()->setupDefaultGeometry(index);
	++index;
	connect(container, SIGNAL(destroyed(QObject*)), m_actionManager, SLOT(updateWindowList()));
}

void iRICMainWindow::createGraph2dHybridWindow()
{
	static int index = 1;
	if (index == 10) {
		index = 1;
	}
	ProjectPostProcessors* posts = m_projectData->mainfile()->postProcessors();
	PostProcessorWindowProjectDataItem* item = m_postWindowFactory->factory("graph2dhybridwindow", posts, this);
	Graph2dHybridWindowProjectDataItem* item2 = dynamic_cast<Graph2dHybridWindowProjectDataItem*>(item);
	bool ok = item2->setupInitialSetting();
	if (! ok) {
		delete item;
		return;
	}
	QMdiSubWindow* container = posts->add(item);
	container->show();
	container->setFocus();
	connect(item->window(), SIGNAL(closeButtonClicked()), container, SLOT(close()));

	item->window()->setupDefaultGeometry(index);
	++index;
	connect(container, SIGNAL(destroyed(QObject*)), m_actionManager, SLOT(updateWindowList()));
}

void iRICMainWindow::createGraph2dScatteredWindow()
{
	static int index = 1;
	if (index == 10) {
		index = 1;
	}
	ProjectPostProcessors* posts = m_projectData->mainfile()->postProcessors();
	PostProcessorWindowProjectDataItem* item = m_postWindowFactory->factory("graph2dscatteredwindow", posts, this);
	Graph2dScatteredWindowProjectDataItem* item2 = dynamic_cast<Graph2dScatteredWindowProjectDataItem*>(item);
	bool ok = item2->setupInitialSetting();
	if (! ok) {
		delete item;
		return;
	}
	QMdiSubWindow* container = posts->add(item);
	container->show();
	container->setFocus();
	connect(item->window(), SIGNAL(closeButtonClicked()), container, SLOT(close()));

	item->window()->setupDefaultGeometry(index);
	++index;
	connect(container, SIGNAL(destroyed(QObject*)), m_actionManager, SLOT(updateWindowList()));
}

void iRICMainWindow::openVerificationDialog()
{
	static int index = 1;
	if (index == 10) {
		index = 1;
	}
	ProjectPostProcessors* posts = m_projectData->mainfile()->postProcessors();
	PostProcessorWindowProjectDataItem* item = m_postWindowFactory->factory("graph2dverificationwindow", posts, this);
	Graph2dVerificationWindowProjectDataItem* item2 = dynamic_cast<Graph2dVerificationWindowProjectDataItem*>(item);
	bool ok = item2->setupInitialSetting();
	if (!ok) {
		delete item;
		return;
	}
	QMdiSubWindow* container = posts->add(item);
	container->show();
	container->setFocus();
	connect(item->window(), SIGNAL(closeButtonClicked()), container, SLOT(close()));

	item->window()->setupDefaultGeometry(index);
	++index;
	connect(container, SIGNAL(destroyed(QObject*)), m_actionManager, SLOT(updateWindowList()));
}

void iRICMainWindow::enterModelessDialogMode()
{
	menuBar()->setDisabled(true);
	m_actionManager->mainToolBar()->setDisabled(true);
	QToolBar* t = m_actionManager->animationToolbar();
	if (t != nullptr) {t->setDisabled(true);}
	t = m_actionManager->additionalToolBar();
	if (t != nullptr) {t->setDisabled(true);}
}

void iRICMainWindow::exitModelessDialogMode()
{
	menuBar()->setDisabled(false);
	m_actionManager->mainToolBar()->setDisabled(false);
	QToolBar* t = m_actionManager->animationToolbar();
	if (t != nullptr) {t->setDisabled(false);}
	t = m_actionManager->additionalToolBar();
	if (t != nullptr) {t->setDisabled(false);}
}

void iRICMainWindow::showPreferenceDialog()
{
	PreferenceDialog dialog;
	dialog.exec();

	setupNetworkProxy();
	updateTmsListForAllWindows();
	m_solverConsoleWindow->applyPreferenceSetting();
}

void iRICMainWindow::loadPlugins()
{
	GridImporterFactory::init();
	GridExporterFactory::init();
}

void iRICMainWindow::initSetting()
{
	// Get locale info
	QSettings settings;
	QString loc = settings.value("general/locale", "").value<QString>();
	if (loc == "") {
		// Language setting is not set. It seems that the user launched iRIC
		// for the first time.

		// locale is set to the system value.
		m_locale = QLocale::system();
		// Set settings with default values.
		PreferenceDialog dialog;
		dialog.save();
	} else {
		m_locale = QLocale(loc);
	}
	QString lastio = settings.value("general/lastiodir").toString();
	if (lastio == "" || ! QDir(lastio).exists()) {
		lastio = QDir::homePath();
	}
	LastIODirectory::set(lastio);

	// for continuous snapshot
	m_continuousSnapshotSetting.exportTargetFolder = QDir(LastIODirectory::get()).filePath("imgs");

	m_metaData = new iRICMetaData(iRIC::toStr(iRICRootPath::get()), m_locale);
}

void iRICMainWindow::setupRecentProjectsMenu()
{
	QMenu* menu = m_actionManager->recentProjectsMenu();
	RecentProjectsManager::setupMenu(menu);

	for (QAction* action : menu->actions()) {
		connect(action, SIGNAL(triggered()), this, SLOT(openRecentProject()));
	}
}

void iRICMainWindow::openRecentProject()
{
	QAction* action = qobject_cast<QAction*>(sender());
	if (action) {
		openProject(action->data().toString());
	}
}

void iRICMainWindow::updateWindowZIndices()
{
	// @todo this fails! investigate the reason.
	int index = 1;

	for (QMdiSubWindow* w : m_centralWidget->subWindowList(QMdiArea::StackingOrder)) {
		WindowWithZIndexInterface* w2 = dynamic_cast<WindowWithZIndexInterface*>(w->widget());
		w2->setZindex(index++);
	}
}

void iRICMainWindow::reflectWindowZIndices()
{
	QMap<int, QMdiSubWindow*> windowsToShow;
	QList<QMdiSubWindow*> wlist = m_centralWidget->subWindowList();
	for (auto it = wlist.begin(); it != wlist.end(); ++it) {
		QMdiSubWindow* w = (*it);
		WindowWithZIndexInterface* w2 = dynamic_cast<WindowWithZIndexInterface*>(w->widget());
		if (w->isVisible()) {
			windowsToShow.insert(w2->zindex(), w);
		}
	}
	for (auto m_it = windowsToShow.begin(); m_it != windowsToShow.end(); ++m_it) {
		QMdiSubWindow* w = m_it.value();
		w->clearFocus();
		w->setFocus();
	}
}

void iRICMainWindow::setupStatusBar()
{
	QStatusBar* sb = statusBar();
	m_mousePositionWidget = new MousePositionWidget(this);
	m_mousePositionWidget->clear();
	sb->addPermanentWidget(m_mousePositionWidget);
}

void iRICMainWindow::updatePostActionStatus()
{
	if (m_projectData == nullptr) {
		m_actionManager->importVisGraphAction->setDisabled(true);
		m_actionManager->importVisGraphActionInCalcMenu->setDisabled(true);
		m_actionManager->windowCreateNew2dPostProcessorAction->setDisabled(true);
		m_actionManager->windowCreateNew2dBirdEyePostProcessorAction->setDisabled(true);
		m_actionManager->windowCreateNew3dPostProcessorAction->setDisabled(true);
		m_actionManager->windowCreateNewGraph2dHybridWindowAction->setDisabled(true);
		m_actionManager->windowCreateNewGraph2dScatteredWindowAction->setDisabled(true);
		m_actionManager->windowCreateVerificationDialogAction->setDisabled(true);
		return;
	}
	PostSolutionInfo* info = m_projectData->mainfile()->postSolutionInfo();
	m_actionManager->importVisGraphAction->setEnabled(info->isDataAvailable());
	m_actionManager->importVisGraphActionInCalcMenu->setEnabled(info->isDataAvailable());
	m_actionManager->windowCreateNew2dPostProcessorAction->setEnabled(info->isDataAvailable2D());
	m_actionManager->windowCreateNew2dBirdEyePostProcessorAction->setEnabled(info->isDataAvailable2D());
	m_actionManager->windowCreateNew3dPostProcessorAction->setEnabled(info->isDataAvailable3D());
	m_actionManager->windowCreateNewGraph2dHybridWindowAction->setEnabled(info->isDataAvailable());
	m_actionManager->windowCreateNewGraph2dScatteredWindowAction->setEnabled(info->isDataAvailable());
	m_actionManager->windowCreateVerificationDialogAction->setEnabled(info->isDataAvailable2D() && m_projectData->mainfile()->measuredDatas().size() > 0);
}

void iRICMainWindow::openHelp()
{
	QDesktopServices::openUrl(QUrl(tr("http://iric-gui-user-manual.readthedocs.io/en/latest/")));
}

void iRICMainWindow::setupAboutDialog()
{
	m_miscDialogManager->setupAboutDialog(*m_metaData);
	m_versionNumber = m_metaData->versionNumber();
}

void iRICMainWindow::openStartDialog()
{
	StartPageDialog dialog(this);
	dialog.setSolverList(m_solverDefinitionList);
	dialog.setLocale(m_locale.name());

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	switch (dialog.commandMode()) {
	case StartPageDialog::cmNewProject:
		newProject(dialog.solverDefinition());
		break;
	case StartPageDialog::cmNewOtherProject:
		newProject();
		break;

	case StartPageDialog::cmOpenProject:
		openProject(dialog.projectFileName());
		break;
	case StartPageDialog::cmOpenOtherProject:
		openProject();
		break;
	}
}

void iRICMainWindow::setDebugMode(bool debug)
{
	m_debugMode = debug;
	if (debug) {
		vtkObject::GlobalWarningDisplayOn();
	} else {
		vtkObject::GlobalWarningDisplayOff();
	}
}

bool iRICMainWindow::isDebugMode() const
{
	return m_debugMode;
}

bool iRICMainWindow::continuousSnapshotInProgress() const
{
	return m_continuousSnapshotInProgress;
}

void iRICMainWindow::setContinuousSnapshotInProgress(bool prog)
{
	m_continuousSnapshotInProgress = prog;
}

void iRICMainWindow::parseArgs()
{
	bool projectSpecified = false;
	QStringList acceptableOptions, unknownOptions, additionalProjects;
	// add acceptable options here.
	acceptableOptions << "-vtkdebug-on";

	QStringList args = QCoreApplication::arguments();
	for (int i = 1; i < args.count(); ++i) {
		QString arg = args.at(i);
		if (arg.left(1) == "-") {
			// It is an argument. check whether it is acceptable.
			int idx = acceptableOptions.indexOf(arg);
			if (idx == - 1) {
				// it is not acceptable.
				unknownOptions << arg;
			}
		} else {
			if (projectSpecified) {
				additionalProjects << arg;
			} else {
				// this is the first project.
				projectSpecified = true;
			}
		}
	}
	if (unknownOptions.count() > 0) {
		// Unknown options found. Show warning window.
		QMessageBox::warning(this, tr("Warning"), tr("Unknown options specified. They are neglected. %1").arg(unknownOptions.join(" ")));
	}
	if (additionalProjects.count() > 0) {
		// More than two projects specified. Show warning window.
		QMessageBox::warning(this, tr("Warning"), tr("More than two project files are passed as arguments. They are neglected. %1").arg(additionalProjects.join(", ")));
	}
	// debug mode setting.
#ifdef _DEBUG
	setDebugMode(true);
#else
	setDebugMode(false);
#endif

	for (int i = 0; i < args.count(); ++i) {
		QString arg = args.at(i);
		if (arg == "-vtkdebug-on") {
			setDebugMode(true);
		}
	}
}

void iRICMainWindow::clearCalculationResult()
{
	if (solverConsoleWindow()->isSolverRunning()) {
		warnSolverRunning();
		return;
	}
	int ret = QMessageBox::warning(this, tr("Warning"), tr("Are you sure you want to delete the calculation result?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if (ret == QMessageBox::No) {return;}

	m_projectData->mainfile()->clearResults();
	m_solverConsoleWindow->clear();

	statusBar()->showMessage(tr("Calculation result cleared."), STATUSBAR_DISPLAYTIME);
}

void iRICMainWindow::manageCalculatedResults()
{
	CalculatedResultManager::openManageDialog(this);
}

void iRICMainWindow::restoreWindowState()
{
	QSettings settings;
	unsigned int state = settings.value("general/windowstate", Qt::WindowNoState).value<unsigned int>();
	setWindowState(static_cast<Qt::WindowStates>(state));
	QByteArray state2 = settings.value("general/windowstate2").value<QByteArray>();
	// restoreState(state2);
	if (! isMaximized()) {
		QSize size = settings.value("general/windowsize", QSize(640, 480)).value<QSize>();
		resize(size);
		QPoint position = settings.value("general/windowposition", QPoint(0, 0)).value<QPoint>();
		move(position);
	}

	// if it is minimized, set normal.
	if (isMinimized()) {showNormal();}

	// Check whether the window is outside of desktop. We should check
	// because maybe user changed desktop resolution to a smaller size,
	// and iRIC mainwindow is out of that size.

	QDesktopWidget* w = QApplication::desktop();
	QRect geom = w->screenGeometry(this);

	bool outside = true;
	QPoint topLeft = pos();
	QPoint topRight = pos() + QPoint(width(), 0);
	QPoint bottomLeft = pos() + QPoint(0, height());
	QPoint bottomRight = pos() + QPoint(width(), height());
	outside = outside && (! geom.contains(topLeft));
	outside = outside && (! geom.contains(topRight));
	outside = outside && (! geom.contains(bottomLeft));
	outside = outside && (! geom.contains(bottomRight));

	if (outside) {
		// out side of the screen!
		move(QPoint(0, 0));
		resize(640, 480);
	}
}

void iRICMainWindow::saveWindowState()
{
	QSettings settings;
	unsigned int state = windowState();
	settings.setValue("general/windowstate", state);
	QByteArray state2 = saveState();
	settings.setValue("general/windowstate2", state2);
	QSize s = size();
	settings.setValue("general/windowsize", s);
	QPoint position = pos();
	settings.setValue("general/windowposition", position);
}


void iRICMainWindow::exportCalculationResult()
{
	if (m_solverConsoleWindow->isSolverRunning()) {
		warnSolverRunning();
		return;
	}
	if (! m_projectData->mainfile()->postSolutionInfo()->hasResults()) {
		QMessageBox::information(this, tr("Information"), tr("Calculation result does not exists."));
		return;
	}
	m_projectData->mainfile()->postSolutionInfo()->exportCalculationResult();
}

void iRICMainWindow::exportParticles()
{
	if (m_solverConsoleWindow->isSolverRunning()) {
		warnSolverRunning();
		return;
	}
	ParticleExportWindowI* ew = dynamic_cast<ParticleExportWindowI*>(m_centralWidget->activeSubWindow()->widget());
	if (ew == nullptr) {
		QMessageBox::information(this, tr("Information"), tr("Please select this menu when Visualization Window is active."));
		return;
	}

	// check whether it has result.
	if (! m_projectData->mainfile()->postSolutionInfo()->hasResults()) {
		QMessageBox::information(this, tr("Information"), tr("Calculation result does not exists."));
		return;
	}

	QList<QString> particleZones = ew->particleDrawingZones();
	QString zoneName;
	if (particleZones.count() == 0) {
		// No valid grid.
		QMessageBox::warning(this, tr("Error"), tr("No particle is drawn now."));
		return;
	} else if (particleZones.count() == 1) {
		zoneName = particleZones.at(0);
	} if (particleZones.count() > 1) {
		ItemSelectingDialog dialog;
		dialog.setItems(particleZones);
		int ret = dialog.exec();
		if (ret == QDialog::Rejected) {
			return;
		}
		zoneName = particleZones.at(dialog.selectedIndex());
	}
	// show setting dialog
	PostDataExportDialog expDialog(this);

	PostSolutionInfo* pInfo = m_projectData->mainfile()->postSolutionInfo();
	expDialog.hideFormat();
	expDialog.setProjectMainFile(m_projectData->mainfile());

	PostExportSetting s = pInfo->exportSetting();
	if (s.folder == "") {
		s.folder = LastIODirectory::get();
	}

	expDialog.setExportSetting(s);
	expDialog.setPrefix(pInfo->particleExportPrefix());
	expDialog.hideDataRange();

	expDialog.setWindowTitle(tr("Export Particles"));

	if (expDialog.exec() != QDialog::Accepted) {return;}

	s = expDialog.exportSetting();
	pInfo->setExportSetting(s);

	pInfo->setParticleExportPrefix(expDialog.prefix());

	// start exporting.
	QProgressDialog dialog(this);
	dialog.setRange(s.startStep, s.endStep);
	dialog.setWindowTitle(tr("Export Particles"));
	dialog.setLabelText(tr("Saving particles as VTK files..."));
	dialog.setFixedSize(300, 100);
	dialog.setModal(true);
	dialog.show();

	ValueChangerT<bool> continuousSnapshotInProgressChanger(&m_continuousSnapshotInProgress, true);

	int step = s.startStep;
	int fileIndex = 1;
	QDir outputFolder(s.folder);
	while (step <= s.endStep) {
		dialog.setValue(step);
		qApp->processEvents();
		if (dialog.wasCanceled()) {return;}
		m_animationController->setCurrentStepIndex(step);
		QString prefixName = pInfo->particleExportPrefix();
		double time = m_projectData->mainfile()->postSolutionInfo()->currentTimeStep();
		bool ok = ew->exportParticles(outputFolder.absoluteFilePath(prefixName), fileIndex, time, zoneName);
		if (! ok) {
			QMessageBox::critical(this, tr("Error"), tr("Error occured while saving."));
			return;
		}
		step += s.skipRate;
		++ fileIndex;
	}
}

void iRICMainWindow::exportCfShape()
{
	if (m_solverConsoleWindow->isSolverRunning()) {
		warnSolverRunning();
		return;
	}
	CfShapeExportWindowI* ew = dynamic_cast<CfShapeExportWindowI*>(m_centralWidget->activeSubWindow()->widget());
	if (ew == nullptr) {
		QMessageBox::information(this, tr("Information"), tr("Currently active sub-window does not support exporting contour figure."));
		return;
	}

	// check whether it has result.
	if (! m_projectData->mainfile()->postSolutionInfo()->hasResults()) {
		QMessageBox::information(this, tr("Information"), tr("Calculation result does not exists."));
		return;
	}

	QList<QString> zones = ew->contourFigureDrawingZones();
	QString zoneName;
	if (zones.count() == 0) {
		// No valid grid.
		QMessageBox::warning(this, tr("Error"), tr("To export shape file, switch \"Display setting\" to \"Contour Figure\", not \"Color Fringe\" or \"Isolines\"."));
		return;
	} else if (zones.count() == 1) {
		zoneName = zones.at(0);
	} if (zones.count() > 1) {
		ItemSelectingDialog dialog;
		dialog.setItems(zones);
		int ret = dialog.exec();
		if (ret == QDialog::Rejected) {
			return;
		}
		zoneName = zones.at(dialog.selectedIndex());
	}
	if (Post2dWindow* pw = dynamic_cast<Post2dWindow*>(m_centralWidget->activeSubWindow()->widget())) {
		if (! pw->checkShapeExportCondition(zoneName)) {return;}
	}
	// show setting dialog
	PostDataExportDialog expDialog(this);

	PostSolutionInfo* pInfo = m_projectData->mainfile()->postSolutionInfo();
	expDialog.hideFormat();
	expDialog.setProjectMainFile(m_projectData->mainfile());

	PostExportSetting s = pInfo->exportSetting();
	if (s.folder == "") {
		s.folder = LastIODirectory::get();
	}

	expDialog.setExportSetting(s);
	expDialog.hideDataRange();

	expDialog.setWindowTitle(tr("Export contour figure to ESRI Shapefiles"));

	if (expDialog.exec() != QDialog::Accepted) {return;}

	s = expDialog.exportSetting();
	pInfo->setExportSetting(s);

	// start exporting.
	QProgressDialog dialog(this);
	dialog.setRange(s.startStep, s.endStep);
	dialog.setWindowTitle(tr("Export contour figure"));
	dialog.setLabelText(tr("Saving contour figure as ESRI Shapefiles..."));
	dialog.setFixedSize(300, 100);
	dialog.setModal(true);
	dialog.show();

	ValueChangerT<bool> continuousSnapshotInProgressChanger(&m_continuousSnapshotInProgress, true);

	int step = s.startStep;
	int fileIndex = 1;
	QDir outputFolder(s.folder);
	while (step <= s.endStep) {
		dialog.setValue(step);
		qApp->processEvents();

		if (dialog.wasCanceled()) {return;}

		m_animationController->setCurrentStepIndex(step);
		QString prefixName = s.prefix;
		double time = m_projectData->mainfile()->postSolutionInfo()->currentTimeStep();
		bool ok = ew->exportContourFigureToShape(outputFolder.absoluteFilePath(prefixName), fileIndex, time, zoneName);
		if (! ok) {
			QMessageBox::critical(this, tr("Error"), tr("Error occured while saving."));
			return;
		}
		step += s.skipRate;
		++ fileIndex;
	}
}

void iRICMainWindow::exportStKMZ()
{
	static QString outputFileName;
	if (outputFileName == "") {
		QDir dir(LastIODirectory::get());
		outputFileName = dir.absoluteFilePath("output.kmz");
	}

	if (m_solverConsoleWindow->isSolverRunning()) {
		warnSolverRunning();
		return;
	}
	SVKmlExportWindowI* ew = dynamic_cast<SVKmlExportWindowI*>(m_centralWidget->activeSubWindow()->widget());
	if (ew == nullptr) {
		QMessageBox::information(this, tr("Information"), tr("Please select this menu when Visualization Window is active."));
		return;
	}

	// check whether it has result.
	if (! m_projectData->mainfile()->postSolutionInfo()->hasResults()) {
		QMessageBox::information(this, tr("Information"), tr("Calculation result does not exists."));
		return;
	}

	QList<QString> zones = ew->contourDrawingZones();
	QString zoneName;
	if (zones.count() == 0) {
		// No valid grid.
		QMessageBox::warning(this, tr("Error"), tr("No Contour Figure is drawn now."));
		return;
	} else if (zones.count() == 1) {
		zoneName = zones.at(0);
	} if (zones.count() > 1) {
		ItemSelectingDialog dialog;
		dialog.setMessage(tr("Please select which zone to use:"));
		dialog.setItems(zones);
		int ret = dialog.exec();
		if (ret == QDialog::Rejected) {
			return;
		}
		zoneName = zones.at(dialog.selectedIndex());
	}
	if (! ew->checkKmlExportCondition(zoneName)) {return;}

	// show setting dialog
	PostDataExportDialog expDialog(this);

	PostSolutionInfo* pInfo = m_projectData->mainfile()->postSolutionInfo();
	expDialog.hideFormat();
	expDialog.hideDataRange();
	expDialog.setFileMode();
	expDialog.setProjectMainFile(m_projectData->mainfile());
	PostExportSetting s = pInfo->exportSetting();
	s.filename = outputFileName;
	expDialog.setExportSetting(s);
	expDialog.setWindowTitle(tr("Export Google Earth KMZ for street view"));

	if (expDialog.exec() != QDialog::Accepted) {return;}

	s = expDialog.exportSetting();
	pInfo->setExportSetting(s);

	outputFileName = s.filename;
	if (QFile::exists(outputFileName)) {
		bool ok = QFile(outputFileName).remove();
		if (! ok) {
			QMessageBox::critical(this, tr("Error"), tr("%1 can not be overwritten.").arg(QDir::toNativeSeparators(outputFileName)));
			return;
		}
	}

	QString tmpKmlName = m_workspace->workspace().absoluteFilePath("doc.kml");
	QFile mainKML(tmpKmlName);
	bool ok = mainKML.open(QFile::WriteOnly);
	QXmlStreamWriter w(&mainKML);
	w.setAutoFormatting(true);
	w.writeStartDocument("1.0");
	w.writeDefaultNamespace("http://earth.google.com/kml/2.2");
	w.writeStartElement("kml");

	ok = ew->exportKMLHeader(w, zoneName);
	if (! ok) {return;}

	// start exporting.
	QProgressDialog dialog(this);
	dialog.setRange(s.startStep, s.endStep);
	dialog.setWindowTitle(tr("Export Google Earth KMZ for street view"));
	dialog.setLabelText(tr("Saving KMZ file..."));
	dialog.setFixedSize(300, 100);
	dialog.setModal(true);
	dialog.show();

	ValueChangerT<bool> continuousSnapshotInProgressChanger(&m_continuousSnapshotInProgress, true);

	int step = s.startStep;
	int fileIndex = 1;
	bool oneShot = (s.startStep == s.endStep);
	while (step <= s.endStep) {
		dialog.setValue(step);
		qApp->processEvents();

		if (dialog.wasCanceled()) {return;}

		m_animationController->setCurrentStepIndex(step);
		double time = m_projectData->mainfile()->postSolutionInfo()->currentTimeStep();
		bool ok = ew->exportKMLForTimestep(w, fileIndex, time, zoneName, oneShot);
		if (! ok) {
			QMessageBox::critical(this, tr("Error"), tr("Error occured while saving."));
			return;
		}
		step += s.skipRate;
		++ fileIndex;
	}

	ok = ew->exportKMLFooter(w, zoneName);

	w.writeEndElement();
	w.writeEndDocument();
	mainKML.close();

	QString tmpZipName = m_workspace->tmpFileName();
	tmpZipName.append(".zip");
	QStringList list;
	list << "doc.kml";
	iRIC::ZipArchive(tmpZipName, m_workspace->workspace().absolutePath(), list);
	ok = QFile::rename(tmpZipName, outputFileName);
	mainKML.remove();

	statusBar()->showMessage(tr("Google Earth KMZ is exported to %1 successfully.").arg(QDir::toNativeSeparators(outputFileName)), STATUSBAR_DISPLAYTIME);
}

QString iRICMainWindow::tmpFileName(int len) const
{
	QCryptographicHash hash(QCryptographicHash::Md5);
	QTime current = QTime::currentTime();
	qsrand(current.msec());
	hash.addData(QByteArray(1, qrand()));
	QDir workDir(QCoreApplication::instance()->applicationDirPath());

	QString filename = hash.result().toHex();
	if (len != 0) {
		filename = filename.left(len);
	}
	while (workDir.exists(filename)) {
		hash.addData(QByteArray(1, qrand()));
		filename = hash.result().toHex();
	}
	return workDir.absoluteFilePath(filename);
}

AnimationControllerInterface* iRICMainWindow::animationController() const
{
	return m_animationController;
}

CoordinateSystemBuilder* iRICMainWindow::coordinateSystemBuilder() const
{
	return m_coordinateSystemBuilder;
}

void iRICMainWindow::checkCgnsStepsUpdate()
{
	if (m_projectData == nullptr) {return;}
	if (! m_projectData->isSolverRunning()) {return;}

	CursorChanger cursorChanger(QCursor(Qt::WaitCursor), this);
	m_projectData->mainfile()->postSolutionInfo()->close();
	QFile::remove(m_projectData->flushCopyCgnsFileName());

	bool ok = FlushRequester::requestFlush(m_projectData->workDirectory(), this);
	if (! ok) {return;}

	m_projectData->mainfile()->postSolutionInfo()->checkCgnsStepsUpdate();
}

void iRICMainWindow::importMeasuredData()
{
	m_projectData->mainfile()->addMeasuredData();
	updatePostActionStatus();
}

void iRICMainWindow::launchExternalTool()
{
	QAction* a = dynamic_cast<QAction*> (sender());
	for (SolverDefinitionAbstract* tool : m_guiToolList->solverList()) {
		if (tool->caption() == a->text()) {
			SolverDefinition def(tool->folder().absolutePath(), m_locale);
			QProcess::startDetached(QString("\"%1\"").arg(def.executableFilename()));
			return;
		}
	}
}

void iRICMainWindow::importVisGraphSetting()
{
	QString fname = QFileDialog::getOpenFileName(
		this, tr("Import Visualization/Graph Settings"), LastIODirectory::get(), tr("Setting file (*.vgsetting)")
		);
	if (fname == "") {return;}

	// check whether the file exists.
	if (! QFile::exists(fname)) {
		// the CGNS file does not exists!
		QMessageBox::warning(this, tr("Warning"), tr("File %1 does not exists.").arg(fname));
		return;
	}

	bool ok = m_projectData->mainfile()->importVisGraphSetting(fname);

	if (ok) {
		QFileInfo info(fname);
		LastIODirectory::set(info.absolutePath());
	}
}

void iRICMainWindow::exportVisGraphSetting()
{
	QString fname = QFileDialog::getSaveFileName(
		this, tr("Export Visualization/Graph Settings"), LastIODirectory::get(), tr("Setting file (*.vgsetting)")
		);
	if (fname == "") {return;}

	bool ok = m_projectData->mainfile()->exportVisGraphSetting(fname);

	if (ok) {
		QFileInfo info(fname);
		LastIODirectory::set(info.absolutePath());
	}
}

void iRICMainWindow::tileSubWindows()
{
	m_centralWidget->tileSubWindows();
	m_centralWidget->hide();
	m_centralWidget->show();
}

void iRICMainWindow::initForSolverDefinition()
{
	// initializes pre-processor for the specified solver definition.
	PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*>(m_preProcessorWindow);
	pre->projectDataItem()->initForSolverDefinition();
	// initializes solver console window for the specified solver definition.
	m_solverConsoleWindow->projectDataItem()->initForSolverDefinition();
}

void iRICMainWindow::loadSubWindowsFromProjectMainFile(const QDomNode& node)
{
	// read setting about PreProcessor
	QDomNode tmpNode = iRIC::getChildNode(node, "PreProcessorWindow");
	if (! tmpNode.isNull()) {
		PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*>(m_preProcessorWindow);

		auto tmpElem = tmpNode.toElement();
		auto ref = tmpElem.attribute("ref");
		if (ref != "") {
			// file is saved in separate xml file
			QDir workDir(projectData()->workDirectory());
			QString fname = workDir.filePath(ref);
			if (QFile::exists(fname)) {
				QFile f(fname);
				QDomDocument doc;
				bool ok = doc.setContent(&f);
				if (ok) {
					pre->projectDataItem()->loadFromProjectMainFile(doc.documentElement());
				}
			}
		} else {
			pre->projectDataItem()->loadFromProjectMainFile(tmpNode);
		}
	}
	// read setting about Console Window
	tmpNode = iRIC::getChildNode(node, "SolverConsoleWindow");
	if (! tmpNode.isNull()) {
		m_solverConsoleWindow->projectDataItem()->loadFromProjectMainFile(tmpNode);
	}

	// read setting abound Post processors
	tmpNode = iRIC::getChildNode(node, "PostProcessorFactory");
	if (! tmpNode.isNull()) {
		m_postWindowFactory->loadWindowCounts(tmpNode);
	}
}

void iRICMainWindow::saveSubWindowsToProjectMainFile(QXmlStreamWriter& writer)
{
	auto xmlPath = QString("%1/preprocessor.xml").arg(ProjectPostProcessors::subwindowsFolder());
	// write setting about PreProcessor
	writer.writeStartElement("PreProcessorWindow");
	writer.writeAttribute("ref", xmlPath);
	writer.writeEndElement();

	QDir workDir(m_projectData->workDirectory());
	QFile f(workDir.filePath(xmlPath));
	f.open(QFile::WriteOnly);
	QXmlStreamWriter writer2(&f);
	writer2.setAutoFormatting(true);
	writer2.writeStartDocument("1.0");
	writer2.writeStartElement("PreProcessorWindow");

	auto pre = dynamic_cast<PreProcessorWindow*>(m_preProcessorWindow);
	pre->projectDataItem()->saveToProjectMainFile(writer2);

	writer2.writeEndElement();
	writer2.writeEndDocument();
	f.close();

	// write setting about SolverConsoleWindow
	writer.writeStartElement("SolverConsoleWindow");
	// delegate to SolverConsoleWindowProjectDataItem
	m_solverConsoleWindow->projectDataItem()->saveToProjectMainFile(writer);
	writer.writeEndElement();

	writer.writeStartElement("PostProcessorFactory");
	m_postWindowFactory->saveWindowCounts(writer);
	writer.writeEndElement();
}

QStringList iRICMainWindow::containedFiles() const
{
	PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*>(m_preProcessorWindow);
	auto files = pre->projectDataItem()->containedFiles();

	auto xmlPath = QString("%1/preprocessor.xml").arg(ProjectPostProcessors::subwindowsFolder());
	files.append(xmlPath);

	return files;
}

int iRICMainWindow::loadFromCgnsFile()
{
	auto pre = dynamic_cast<PreProcessorWindow*>(m_preProcessorWindow);
	return pre->projectDataItem()->loadFromCgnsFile();
}

int iRICMainWindow::saveToCgnsFile()
{
	auto pre = dynamic_cast<PreProcessorWindow*>(m_preProcessorWindow);
	return pre->projectDataItem()->saveToCgnsFile();
}

int iRICMainWindow::updateCgnsFileOtherThanGrids()
{
	auto pre = dynamic_cast<PreProcessorWindow*>(m_preProcessorWindow);
	return pre->projectDataItem()->updateCgnsFileOtherThanGrids();
}

void iRICMainWindow::closeCgnsFile()
{
	PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*>(m_preProcessorWindow);
	pre->projectDataItem()->closeCgnsFile();
}

void iRICMainWindow::toggleGridEditFlag()
{
	PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*>(m_preProcessorWindow);
	pre->projectDataItem()->setGridEdited();
}

ProjectData* iRICMainWindow::projectData() const
{
	return m_projectData;
}

void iRICMainWindow::setProjectData(ProjectData* projectData)
{
	projectData->mainfile()->postProcessors()->setFactory(m_postWindowFactory);
	PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*>(m_preProcessorWindow);
	pre->setProjectData(projectData);
	m_solverConsoleWindow->setProjectData(projectData);
	m_actionManager->setProjectData(projectData);
}

bool iRICMainWindow::checkWorkFolderWorks()
{
	QString path = m_workspace->workspace().absolutePath();
	if (! iRIC::isAscii(path)) {
		QMessageBox::warning(this, tr("Warning"), tr(
									"Current working directory (%1) contains non-ASCII characters. "
									"Before starting a new project, change working directory from the following menu: \n"
									"Option -> Preferences").arg(QDir::toNativeSeparators(path)));
		return false;
	}
	return true;
}

void iRICMainWindow::setupProcessEnvironment()
{
	m_processEnvironment = QProcessEnvironment::systemEnvironment();
	QDir execDir = QDir(qApp->applicationDirPath());
	QString path = QDir::toNativeSeparators(execDir.absolutePath());
	path.append(";");
	path.append(m_processEnvironment.value("PATH"));
	m_processEnvironment.insert("PATH", path);
}

void iRICMainWindow::setupNetworkProxy()
{
	NetworkSetting setting;

	auto proxies = setting.queryProxy();
	if (proxies.size() == 0) {return;}

	QNetworkProxy::setApplicationProxy(proxies.first());
}

void iRICMainWindow::updateTmsListForAllWindows()
{
	for (auto w : m_centralWidget->subWindowList()) {
		auto innerWindow = w->widget();
		auto tmsW = dynamic_cast<WindowWithTmsI*> (innerWindow);
		if (tmsW == nullptr) {continue;}

		tmsW->updateTmsList();
	}
}
