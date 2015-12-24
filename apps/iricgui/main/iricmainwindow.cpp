#include "../continuoussnapshot/continuoussnapshotmoviepropertypage.h"
#include "../continuoussnapshot/continuoussnapshotwizard.h"
#include "../factory/postprocessorwindowfactory.h"
#include "../factory/postprocessorwindowfactory.h"
#include "../googlemapimport/googlemapimageimporter.h"
#include "../misc/animationcontroller.h"
#include "../misc/iricmainwindowactionmanager.h"
#include "../misc/iricmainwindowmiscdialogmanager.h"
#include "../misc/newprojectsolverselectingdialog.h"
#include "../misc/projecttypeselectdialog.h"
#include "../pref/preferencedialog.h"
#include "../projectproperty/projectpropertydialog.h"
#include "../solverdef/solverdefinitionlist.h"
#include "../startpage/startpagedialog.h"
#include "../verification/verificationgraphdialog.h"
#include "iricmainwindow.h"

#include <gridcreatingcondition/externalprogram/gridcreatingconditioncreatorexternalprogram.h>
#include <guibase/cursorchanger.h>
#include <guibase/coordinatesystembuilder.h>
#include <guibase/irictoolbar.h>
#include <guibase/itemselectingdialog.h>
#include <guicore/base/clipboardoperatablewindowinterface.h>
#include <guicore/base/windowwithzindexinterface.h>
#include <postbase/particleexportwindowinterface.h>
#include <postbase/svkmlexportwindowinterface.h>
#include <guicore/misc/mousepositionwidget.h>
#include <guicore/post/postprocessorwindowprojectdataitem.h>
#include <guicore/postcontainer/postdataexportdialog.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/posttimesteps.h>
#include <guicore/project/cgnsfilelist.h>
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
#include <misc/lastiodirectory.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>
#include <postbase/cfshapeexportwindowinterface.h>
#include <postbase/particleexportwindowinterface.h>
#include <post/graph2dhybrid/graph2dhybridwindowprojectdataitem.h>
#include <post/graph2dscattered/graph2dscatteredwindowprojectdataitem.h>
#include <post/post2d/post2dwindow.h>
#include <pre/factory/gridcreatingconditionfactory.h>
#include <pre/factory/gridexporterfactory.h>
#include <pre/factory/gridimporterfactory.h>
#include <pre/preprocessorwindow.h>
#include <pre/preprocessorwindowprojectdataitem.h>
#include <geodata/riversurvey/geodatariversurveycrosssectionwindow.h>
#include <solverconsole/solverconsolewindow.h>
#include <solverconsole/solverconsolewindowprojectdataitem.h>

#include <QApplication>
#include <QCryptographicHash>
#include <QDomNode>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QInputDialog>
#include <QLabel>
#include <QMap>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPainter>
#include <QProgressDialog>
#include <QRegExp>
#include <QSettings>
#include <QSlider>
#include <QStatusBar>
#include <QThread>
#include <QTime>
#include <QVector2D>
#include <QXmlStreamWriter>

#include <vtkGL2PSExporter.h>
#include <vtkObject.h>
#include <vtkSmartPointer.h>

class SolverDefinitionAbstract;

const int iRICMainWindow::MAX_RECENT_PROJECTS = 10;
const int iRICMainWindow::MAX_RECENT_SOLVERS = 10;

iRICMainWindow::iRICMainWindow(QWidget* parent) :
	iRICMainWindowInterface(parent),
	m_workspace {new ProjectWorkspace {this}},
	m_metaData {this},
	m_projectData {nullptr},
	m_postWindowFactory {new PostProcessorWindowFactory {this}},
	m_coordinateSystemBuilder {new CoordinateSystemBuilder {this}},
	m_miscDialogManager {new iRICMainWindowMiscDialogManager {this}},
	m_animationController {new AnimationController {this}}
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

	setupBasicSubWindows();

	// setup action manager
	m_actionManager = new iRICMainWindowActionManager(this);
	m_actionManager->projectFileClose();
	connect(m_animationController, SIGNAL(indexChanged(uint)), this, SLOT(setCurrentStep(uint)));
	connect(this, SIGNAL(allPostProcessorsUpdated()), m_animationController, SLOT(handleRenderingEnded()));

	setMenuBar(m_actionManager->menuBar());
	addToolBar(m_actionManager->mainToolBar());
	addToolBar(m_actionManager->windowsToolBar());
	addToolBarBreak(Qt::TopToolBarArea);

	// Setup solver definition list.
	m_solverDefinitionList = new SolverDefinitionList(QCoreApplication::instance()->applicationDirPath(), m_locale, this);
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
	saveWindowState();
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

	QSettings settings;
	QStringList recentSolvers = settings.value("general/recentsolvers", QStringList()).toStringList();
	if (recentSolvers.count() > 0) {
		QString firstSolver = recentSolvers.at(0);
		dialog.setSolver(firstSolver);
	}
	if (dialog.exec() == QDialog::Accepted) {
		SolverDefinitionAbstract* selectedSolver = dialog.selectedSolver();
		updateRecentSolvers(selectedSolver->folderName());
		newProject(selectedSolver);
	}
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
	updateRecentSolvers(solver->folderName());

	// create solver definition data
	QString solFolder = m_solverDefinitionList->absoluteSolverPath(solver->folderName());
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
	m_projectData->mainfile()->createDefaultCgnsFile();
	setupForNewProjectData();

	m_projectData->switchToDefaultCgnsFile();
	connect(m_projectData->mainfile()->postSolutionInfo(), SIGNAL(allPostProcessorsUpdated()), this, SIGNAL(allPostProcessorsUpdated()));
	connect(m_projectData->mainfile()->postSolutionInfo(), SIGNAL(updated()), this, SLOT(updatePostActionStatus()));
	connect(m_actionManager->openWorkFolderAction, SIGNAL(triggered()), m_projectData, SLOT(openWorkDirectory()));

	// show pre-processor window first.
	PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*>(m_preProcessorWindow);
	pre->setupDefaultGeometry();
	pre->parentWidget()->show();
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
		QMessageBox::warning(this, tr("Warning"), tr("Project file %1 does not exists.").arg(filename));
		removeFromRecentProjects(filename);
		return;
	}

	// close project first.
	if (! closeProject()) {return;}

	CursorChanger cursorChanger(QCursor(Qt::WaitCursor), this);
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
			if (! m_projectData->copyTo(wFolder, true)) {
				// copying failed or canceled.
				closeProject();
				return;
			}
		} else {
			if (! iRIC::isAscii(filename)) {
				QMessageBox::warning(this, tr("Warning"), tr("Project folder path has to consist of only English characters. Please move or rename the project folder."));
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
	QString folder = m_solverDefinitionList->supportingSolverFolder(m_projectData);
	QString solFolder;
	if (folder.isNull()){
		m_projectData->setPostOnlyMode();
		solFolder = ":/data/unknownsolver";
	} else {
		solFolder = m_solverDefinitionList->absoluteSolverPath(folder);
	}
	SolverDefinition* def = new SolverDefinition(solFolder, m_locale);
	m_projectData->setSolverDefinition(def);

	try {
		setCursor(Qt::WaitCursor);
		m_projectData->load();
	} catch (ErrorMessage& m) {
		QMessageBox::warning(this, tr("Error"), m);
		closeProject();
		return;
	}

	m_projectData->setVersion(m_versionNumber);
	setupForNewProjectData();

	bool ok = m_projectData->switchToDefaultCgnsFile();
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

	m_isOpening = false;
	LastIODirectory::set(QFileInfo(filename).absolutePath());
	m_projectData->mainfile()->clearModified();
	m_mousePositionWidget->setProjectData(m_projectData);

	// update recently opened projects.
	updateRecentProjects(filename);
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
		this, tr("Open Calculation result"), LastIODirectory::get(), tr("CGNS file (*.cgn)")
		);
	if (fname == "") {return;}
	importCalculationResult(fname);
}

void iRICMainWindow::importCalculationResult(const QString& fname)
{
	// check whether the project file exists.
	if (! QFile::exists(fname)) {
		// the CGNS file does not exists!
		QMessageBox::warning(this, tr("Warning"), tr("CGNS file %1 does not exists.").arg(fname));
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
		QString folder = m_solverDefinitionList->supportingSolverFolder(m_projectData);
		if (folder.isNull()){
			m_projectData->setPostOnlyMode();
			solFolder = ":/data/unknownsolver";
		} else {
			// create solver definition data
			solFolder = m_solverDefinitionList->absoluteSolverPath(folder);
		}
	}
	SolverDefinition* def = new SolverDefinition(solFolder, m_locale);
	m_projectData->setSolverDefinition(def);
	m_projectData->setVersion(m_versionNumber);

	setupForNewProjectData();

	QString newname = m_projectData->mainfile()->cgnsFileList()->proposeFilename();
	bret = m_projectData->mainfile()->importCgnsFile(fname, newname);
	if (! bret) {return;}

	connect(m_projectData->mainfile()->postSolutionInfo(), SIGNAL(allPostProcessorsUpdated()), this, SIGNAL(allPostProcessorsUpdated()));
	connect(m_projectData->mainfile()->postSolutionInfo(), SIGNAL(updated()), this, SLOT(updatePostActionStatus()));
	connect(m_actionManager->openWorkFolderAction, SIGNAL(triggered()), m_projectData, SLOT(openWorkDirectory()));

	// show pre-processor window first.
	if (! m_projectData->isPostOnlyMode()) {
		// show pre-processor window first.
		PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*> (m_preProcessorWindow);
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
	m_preProcessorWindow->parentWidget()->hide();
	m_solverConsoleWindow->parentWidget()->hide();
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
	AnimationController* ac = dynamic_cast<AnimationController*>(m_animationController);
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
	connect(innerWindow, SIGNAL(worldPositionChangedForStatusBar(QVector2D)), m_mousePositionWidget, SLOT(updatePosition(QVector2D)));
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
	if (m_projectData->isPostOnlyMode()) {
		QMessageBox::information(this, tr("Information"), tr("This project is opened in post only mode. You can not save."));
		return false;
	}
	m_isSaving = true;
	bool ret;
	CursorChanger cursorChanger(QCursor(Qt::WaitCursor), this);
	// save data to work folder.
	ret = m_projectData->save();
	if (! ret) {
		QMessageBox::critical(this, tr("Error"), tr("Saving project failed."));
		m_isSaving = false;
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
		if (m_projectData->hasHugeCgns()) {
			QMessageBox::critical(this, tr("Error"), tr("This project has HUGE calculation result, so it cannot saved as a file (*.ipro). Please save as a project."));
			m_isSaving = false;
			return false;
		}
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
		m_isSaving = false;
		return false;
	}
	updateWindowTitle();
	LastIODirectory::set(QFileInfo(filename).absolutePath());
	updateRecentProjects(filename);
	statusBar()->showMessage(tr("Project successfully saved to %1.").arg(QDir::toNativeSeparators(filename)), STATUSBAR_DISPLAYTIME);
	m_isSaving = false;
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

void iRICMainWindow::importGoogleMapImages()
{
	GoogleMapImageImporter importer(m_projectData, this);
	importer.importImages();
}

void iRICMainWindow::showProjectPropertyDialog()
{
	ProjectPropertyDialog dialog(this);
	dialog.setProjectData(m_projectData);
	dialog.exec();
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
	if (enabledWindow != nullptr) {
		enabledWindow->setTransparent(false);
		QPixmap pixmap = enabledWindow->snapshot();
		QString defaultname = QDir(LastIODirectory::get()).absoluteFilePath("snapshot");
		QString filename = QFileDialog::getSaveFileName(this, tr("Save Snapshot"), defaultname, tr("PNG files (*.png);;JPEG file (*.jpg);;Windows BMP file (*.bmp);;Encapsulated Post Script file (*.eps);;Portable Document Format file (*.pdf);;Scalable Vector Graphics file (*.svg)"));
		if (filename == "") {return;}

		QFileInfo finfo(filename);
		if (finfo.suffix() == "jpg" || finfo.suffix() == "png" || finfo.suffix() == "bmp") {
			pixmap.save(filename);
		} else if (finfo.suffix() == "pdf" || finfo.suffix() == "eps" || finfo.suffix() == "svg") {
			vtkRenderWindow* renderWindow = enabledWindow->getVtkRenderWindow();
			if (renderWindow == nullptr) {
				QMessageBox::warning(this, tr("Warning"), tr("This window do not support snapshot with this file type."));
				return;
			}
			vtkSmartPointer<vtkGL2PSExporter> exp = vtkSmartPointer<vtkGL2PSExporter>::New();
			exp->SetRenderWindow(renderWindow);
			exp->CompressOff();
			exp->Write3DPropsAsRasterImageOff();
			exp->TextOn();
			if (finfo.suffix() == "pdf") {
				exp->SetFileFormatToPDF();
			} else if (finfo.suffix() == "eps") {
				exp->SetFileFormatToEPS();
			} else if (finfo.suffix() == "svg") {
				exp->SetFileFormatToSVG();
			}
			QString tmppath = m_projectData->tmpFileName();
			exp->SetFilePrefix(iRIC::toStr(tmppath).c_str());
			exp->Write();
			QString tmpname = QString("%1.%2").arg(tmppath).arg(finfo.suffix());
			QFile tempFile(tmpname);
			if (! tempFile.exists()) {
				QMessageBox::critical(this, tr("Error"), tr("Saving snapshot failed."));
				return;
			}
			QFile newFile(filename);
			newFile.remove();

			bool ok = QFile::rename(tmpname, filename);
			if (! ok) {
				QMessageBox::critical(this, tr("Error"), tr("Saving snapshot failed."));
				return;
			}
		} else {
			QMessageBox::warning(this, tr("Warning"), tr("Wrong file name specified."));
			return;
		}
		statusBar()->showMessage(tr("Snapshot successfully saved to %1.").arg(filename), STATUSBAR_DISPLAYTIME);
		LastIODirectory::set(QFileInfo(filename).absolutePath());
	} else {
		QMessageBox::warning(this, tr("Warning"), tr("This windows does not support snapshot function."));
	}
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

		wizard->setOutput(m_output);
		wizard->setLayout(m_layout);
		wizard->setTransparent(m_transparent);
		wizard->setFileIODirectory(m_directory);
		wizard->setExtension(m_extension);
		wizard->setSuffixLength(m_suffixLength);
		wizard->setOutputMovie(m_outputMovie);
		wizard->setMovieLengthMode(m_movieLengthMode);
		wizard->setMovieLength(m_movieLength);
		wizard->setFramesPerSecond(m_framesPerSecond);
		wizard->setMovieProfile(m_movieProfile);
		wizard->setTimesteps(m_projectData->mainfile()->postSolutionInfo()->timeSteps()->timesteps());
		wizard->setStart(m_start);
		wizard->setStop(m_stop);
		wizard->setSamplingRate(m_samplingRate);
		wizard->setGoogleEarth(m_googleEarth);
		wizard->setLeftLatitude(m_leftLatitude);
		wizard->setLeftLongitude(m_leftLongitude);
		wizard->setRightLatitude(m_rightLatitude);
		wizard->setRightLongitude(m_rightLongitude);
		wizard->setTargetWindow(0);
		wizard->setKMLFilename(m_kmlFilename);
		wizard->setBackgroundList(m_projectData->mainfile()->backgroundImages());
		wizard->setAngle(m_angle);
		wizard->setNorth(m_north);
		wizard->setSouth(m_south);
		wizard->setEast(m_east);
		wizard->setWest(m_west);

		if (wizard->exec() == QDialog::Accepted) {
			handleWizardAccepted(wizard);
			if (m_googleEarth) {
				// opne kml file
				QString kml = QDir(m_directory).absoluteFilePath(m_kmlFilename);
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

void iRICMainWindow::handleWizardAccepted(ContinuousSnapshotWizard* wizard)
{
	m_output = wizard->output();
	m_layout = wizard->layout();
	m_transparent = wizard->transparent();

	m_directory = wizard->fileIODirectory();
	m_extension = wizard->extension();
	m_suffixLength = wizard->suffixLength();

	m_outputMovie = wizard->outputMovie();
	m_movieLengthMode = wizard->movieLengthMode();
	m_movieLength = wizard->movieLength();
	m_framesPerSecond = wizard->framesPerSecond();
	m_movieProfile = wizard->movieProfile();

	m_start = wizard->start();
	m_stop = wizard->stop();
	m_samplingRate = wizard->samplingRate();

	m_googleEarth = wizard->googleEarth();
	m_leftLatitude = wizard->leftLatitude();
	m_leftLongitude = wizard->leftLongitude();
	m_rightLatitude = wizard->rightLatitude();
	m_rightLongitude = wizard->rightLongitude();
	m_kmlFilename = wizard->kmlFilename();
	m_angle = wizard->angle();
	m_north = wizard->north();
	m_south = wizard->south();
	m_east = wizard->east();
	m_west = wizard->west();
}

void iRICMainWindow::saveContinuousSnapshot(ContinuousSnapshotWizard* wizard, QXmlStreamWriter* writer)
{
	m_continuousSnapshotInProgress = true;
	QProgressDialog dialog(wizard);
	dialog.setRange(0, m_stop - m_start);
	dialog.setWindowTitle(tr("Continuous Snapshot"));
	dialog.setLabelText(tr("saving continuous snapshot..."));
	dialog.setFixedSize(300, 100);
	dialog.setModal(true);
	dialog.show();

	int step = m_start;
	auto fit = wizard->fileList().begin();
	QPoint position = wizard->beginPosition();
	QSize size = wizard->snapshotSize();
	QList<QSize> sizes;
	bool first = true;
	int imgCount = 0;
	while (step <= m_stop) {
		dialog.setValue(step - m_start);
		qApp->processEvents();
		if (dialog.wasCanceled()) {
			m_continuousSnapshotInProgress = false;
			return;
		}
		m_animationController->setCurrentStepIndex(step);
		switch (m_output) {
		case ContinuousSnapshotWizard::Onefile: {
				if (first) {sizes.append(size);}
				QImage image = QImage(size, QImage::Format_ARGB32);
				QPixmap pixmap;
				QPainter painter;
				if (m_transparent) {
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
					window->setTransparent(m_transparent);

					switch (m_layout) {
					case ContinuousSnapshotWizard::Asis:
						begin = sub->pos() + sub->widget()->pos() + center->pos();
						painter.drawPixmap(begin.x() - position.x(), begin.y() - position.y(), window->snapshot());
						break;
					case ContinuousSnapshotWizard::Horizontally:
						painter.drawPixmap(begin.x(), begin.y(), window->snapshot());
						begin.setX(begin.x() + center->width());
						break;
					case ContinuousSnapshotWizard::Vertically:
						painter.drawPixmap(begin.x(), begin.y(), window->snapshot());
						begin.setY(begin.y() + center->height());
						break;
					}
				}
				pixmap.save(*fit);
				QString url = QFileInfo(*fit).fileName();
				addKMLElement(step, url, m_north, m_south, m_west, m_east, m_angle, writer);
				++fit;
				break;
			}
		case ContinuousSnapshotWizard::Respectively:
			int i = 0;
			for (QMdiSubWindow* sub : wizard->windowList()) {
				SnapshotEnabledWindowInterface* window = dynamic_cast<SnapshotEnabledWindowInterface*>(sub->widget());
				window->setTransparent(m_transparent);
				QPixmap pixmap = window->snapshot();
				pixmap.save(*fit);
				if (first) {sizes.append(pixmap.size());}
				if (wizard->targetWindow() == i) {
					QString url = QFileInfo(*fit).fileName();
					addKMLElement(step, url, m_north, m_south, m_west, m_east, m_angle, writer);
				}
				++fit;
				++ i;
			}
			break;
		}
		step += m_samplingRate;
		first = false;
		++ imgCount;
	}
	// output Movie.
	if (wizard->outputMovie()) {
		QStringList inputFilenames;
		QStringList outputFilenames;
		QString inputFilename;
		QString outputFilename;
		switch (wizard->output()) {
		case ContinuousSnapshotWizard::Onefile:
			inputFilename = QString("img_%%1d%2").arg(wizard->suffixLength()).arg(wizard->extension());
			outputFilename = QString("img.wmv");
			inputFilenames.append(inputFilename);
			outputFilenames.append(outputFilename);
			break;
		case ContinuousSnapshotWizard::Respectively:
			int idx = 0;
			for (auto it = wizard->windowList().begin(); it != wizard->windowList().end(); ++it) {
				inputFilename = QString("window%1_%%2d%3").arg(idx + 1).arg(wizard->suffixLength()).arg(wizard->extension());
				outputFilename = QString("window%1.wmv").arg(idx + 1);
				inputFilenames.append(inputFilename);
				outputFilenames.append(outputFilename);
				++idx;
			}
			break;
		}
		QStringList profileString = ContinuousSnapshotMoviePropertyPage::getProfile(wizard->movieProfile());
		for (int i = 0; i < inputFilenames.count(); ++i) {
			QString inFile  = inputFilenames.at(i);
			QString outFile = outputFilenames.at(i);
			QString absOutFile = QDir(wizard->fileIODirectory()).absoluteFilePath(outFile);
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
			ffprocess->setWorkingDirectory(wizard->fileIODirectory());
			QStringList args;
			if (m_movieLengthMode == 0) {
				// specify length
				args << "-r" << QString("%1/%2").arg(imgCount).arg(wizard->movieLength());
			} else {
				// specify fps
				args << "-r" << QString("%1").arg(wizard->framesPerSecond());
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
	m_continuousSnapshotInProgress = false;
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
	writer->writeCharacters(QString::number(north));
	writer->writeEndElement();
	writer->writeStartElement("south");
	writer->writeCharacters(QString::number(south));
	writer->writeEndElement();
	writer->writeStartElement("east");
	writer->writeCharacters(QString::number(east));
	writer->writeEndElement();
	writer->writeStartElement("west");
	writer->writeCharacters(QString::number(west));
	writer->writeEndElement();
	writer->writeStartElement("rotation");
	writer->writeCharacters(QString::number(angle));
	writer->writeEndElement();
	writer->writeEndElement();

	writer->writeEndElement();
}

QString iRICMainWindow::timeString(int time)
{
	QString str;
	int hour = time / 3600;
	int minutes = time % 3600;
	int minute = minutes / 60;
	int second = minutes % 60;
	str = QString("2011-01-01T%1:%2:%3").arg(hour, 2, 10, QChar('0'))
				.arg(minute, 2, 10, QChar('0'))
				.arg(second, 2, 10, QChar('0'));
	return str;
}

void iRICMainWindow::updateWindowTitle()
{
	QString fname = "";
	if (m_projectData == nullptr) {
		setWindowTitle(tr("iRIC %1").arg(m_versionNumber.toString()));
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
	QString title = tr("%1 - iRIC %2 [%3]");
	setWindowTitle(title.arg(QDir::toNativeSeparators(fname)).arg(m_versionNumber.toString()).arg(solverName));
}

void iRICMainWindow::warnSolverRunning() const
{
	iRICMainWindow* t = const_cast<iRICMainWindow*> (this);
	QMessageBox::warning(t, tr("Warning"), tr("The solver is running now. Please stop solver, to do this action."), QMessageBox::Ok);
}

bool iRICMainWindow::isSolverRunning() const
{
	return m_solverConsoleWindow->isSolverRunning();
}

void iRICMainWindow::switchCgnsFile(const QString& newcgns)
{
	if (isSolverRunning()) {
		warnSolverRunning();
		return;
	}
	// clear animation tool bar steps.
	AnimationController* ac = dynamic_cast<AnimationController*>(m_animationController);
	ac->clearSteps();
	// switch cgns file.
	m_projectData->mainfile()->switchCgnsFile(newcgns);
	updatePostActionStatus();
}

void iRICMainWindow::setupAnimationToolbar()
{
	connect(m_projectData->mainfile()->postSolutionInfo(), SIGNAL(cgnsStepsUpdated(QList<QString>)), m_animationController, SLOT(updateStepList(QList<QString>)));
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
	connect(m_solverConsoleWindow, SIGNAL(solverStarted()), m_projectData->mainfile()->postSolutionInfo(), SLOT(checkCgnsStepsUpdate()));
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
	VerificationGraphDialog* dialog = new VerificationGraphDialog(this);
	dialog->setPostSolutionInfo(m_projectData->mainfile()->postSolutionInfo());
	dialog->setMeasuredValues(m_projectData->mainfile()->measuredDatas());
	dialog->setModal(true);
	dialog->show();
	bool ok = dialog->setting();
	if (! ok) {
		dialog->close();
	}
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
	m_output = ContinuousSnapshotWizard::Onefile;
	m_layout = ContinuousSnapshotWizard::Asis;
	m_transparent = false;
	m_directory = LastIODirectory::get();
	m_extension = QString(".png");
	m_suffixLength = 4;
	m_outputMovie = false;
	m_movieLengthMode = 0;
	m_movieLength = 10;
	m_framesPerSecond = 5;
	m_movieProfile = 0;
	m_start = -1;
	m_stop = -1;
	m_samplingRate = 1;
	m_googleEarth = false;
	m_leftLatitude = 0;
	m_leftLongitude = 0;
	m_rightLatitude = 0;
	m_rightLongitude = 0;
	m_kmlFilename = QString("output.kml");
	m_angle = 0;
	m_north = 0;
	m_south = 0;
	m_east = 0;
	m_west = 0;
	m_continuousSnapshotInProgress = false;
}

void iRICMainWindow::setupRecentProjectsMenu()
{
	QMenu* menu = m_actionManager->recentProjectsMenu();
	QList<QAction*> actions = menu->actions();
	for (auto a : menu->actions()) {
		delete a;
	}
	menu->clear();
	QSettings setting;
	QStringList recentProjects = setting.value("general/recentprojects", QStringList()).toStringList();
	int numRecentFiles = qMin(recentProjects.size(), MAX_RECENT_PROJECTS);
	for (int i = 0; i < numRecentFiles; ++i) {
		QString text = tr("&%1 %2").arg(i + 1).arg(QDir::toNativeSeparators(recentProjects.at(i)));
		QAction* action = new QAction(text, this);
		action->setData(recentProjects.at(i));
		connect(action, SIGNAL(triggered()), this, SLOT(openRecentProject()));
		menu->addAction(action);
	}
}

void iRICMainWindow::openRecentProject()
{
	QAction* action = qobject_cast<QAction*>(sender());
	if (action) {
		openProject(action->data().toString());
	}
}

void iRICMainWindow::updateRecentProjects(const QString& filename)
{
	QSettings setting;
	QStringList recentProjects = setting.value("general/recentprojects", QStringList()).toStringList();
	recentProjects.removeAll(filename);
	recentProjects.prepend(filename);
	while (recentProjects.size() > MAX_RECENT_PROJECTS) {
		recentProjects.removeLast();
	}
	setting.setValue("general/recentprojects", recentProjects);
}

void iRICMainWindow::removeFromRecentProjects(const QString& filename)
{
	QSettings setting;
	QStringList recentProjects = setting.value("general/recentprojects", QStringList()).toStringList();
	recentProjects.removeAll(filename);
	while (recentProjects.size() > MAX_RECENT_PROJECTS) {
		recentProjects.removeLast();
	}
	setting.setValue("general/recentprojects", recentProjects);
}

void iRICMainWindow::updateRecentSolvers(const QString& foldername)
{
	QSettings setting;
	QStringList recentSolvers = setting.value("general/recentsolvers", QStringList()).toStringList();
	recentSolvers.removeAll(foldername);
	recentSolvers.prepend(foldername);
	while (recentSolvers.size() > MAX_RECENT_SOLVERS) {
		recentSolvers.removeLast();
	}
	setting.setValue("general/recentsolvers", recentSolvers);
}

void iRICMainWindow::removeFromRecentSolvers(const QString& foldername)
{
	QSettings setting;
	QStringList recentSolvers = setting.value("general/recentsolvers", QStringList()).toStringList();
	recentSolvers.removeAll(foldername);
	while (recentSolvers.size() > MAX_RECENT_PROJECTS) {
		recentSolvers.removeLast();
	}
	setting.setValue("general/recentsolvers", recentSolvers);
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
	m_actionManager->windowCreateVerificationDialogAction->setEnabled(info->isDataAvailable2D() && m_projectData->mainfile()->measuredDatas().count() > 0);
}

void iRICMainWindow::setupAboutDialog()
{
	m_miscDialogManager->setupAboutDialog(m_metaData);
	m_versionNumber = m_metaData.versionNumber();
}

void iRICMainWindow::openStartDialog()
{
	StartPageDialog dialog(this);
	dialog.setSolverList(m_solverDefinitionList);
	dialog.setLocale(m_locale.name());
	int ret = dialog.exec();
	SolverDefinitionAbstract* solverDef;
	QString filename;
	if (ret == QDialog::Accepted) {
		switch (dialog.commandMode()) {
		case StartPageDialog::cmNewProject:
			solverDef = dialog.solverDefinition();
			newProject(solverDef);
			break;
		case StartPageDialog::cmNewOtherProject:
			newProject();
			break;

		case StartPageDialog::cmOpenProject:
			filename = dialog.projectFileName();
			openProject(filename);
			break;
		case StartPageDialog::cmOpenOtherProject:
			openProject();
			break;
		}
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
	statusBar()->showMessage(tr("Calculation result cleared."), STATUSBAR_DISPLAYTIME);
}

void iRICMainWindow::restoreWindowState()
{
	QSettings settings;
	unsigned int state = settings.value("general/windowstate", Qt::WindowNoState).value<unsigned int>();
	setWindowState(static_cast<Qt::WindowStates>(state));
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
	ParticleExportWindowInterface* ew = dynamic_cast<ParticleExportWindowInterface*>(m_centralWidget->activeSubWindow()->widget());
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
		zoneName = particleZones.at(dialog.selectIndex());
	}
	// show setting dialog
	PostDataExportDialog expDialog(this);

	PostSolutionInfo* pInfo = m_projectData->mainfile()->postSolutionInfo();
	expDialog.hideFormat();
	expDialog.setTimeValues(pInfo->timeSteps()->timesteps());

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

	m_continuousSnapshotInProgress = true;

	int step = s.startStep;
	int fileIndex = 1;
	QDir outputFolder(s.folder);
	while (step <= s.endStep) {
		dialog.setValue(step);
		qApp->processEvents();
		if (dialog.wasCanceled()) {
			m_continuousSnapshotInProgress = false;
			return;
		}
		m_animationController->setCurrentStepIndex(step);
		QString prefixName = pInfo->particleExportPrefix();
		double time = m_projectData->mainfile()->postSolutionInfo()->currentTimeStep();
		bool ok = ew->exportParticles(outputFolder.absoluteFilePath(prefixName), fileIndex, time, zoneName);
		if (! ok) {
			QMessageBox::critical(this, tr("Error"), tr("Error occured while saving."));
			m_continuousSnapshotInProgress = false;
			return;
		}
		step += s.skipRate;
		++ fileIndex;
	}
	m_continuousSnapshotInProgress = false;
}

void iRICMainWindow::exportCfShape()
{
	if (m_solverConsoleWindow->isSolverRunning()) {
		warnSolverRunning();
		return;
	}
	CfShapeExportWindowInterface* ew = dynamic_cast<CfShapeExportWindowInterface*>(m_centralWidget->activeSubWindow()->widget());
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
		zoneName = zones.at(dialog.selectIndex());
	}
	// show setting dialog
	PostDataExportDialog expDialog(this);

	PostSolutionInfo* pInfo = m_projectData->mainfile()->postSolutionInfo();
	expDialog.hideFormat();
	expDialog.setTimeValues(pInfo->timeSteps()->timesteps());

	PostExportSetting s = pInfo->exportSetting();
	if (s.folder == "") {
		s.folder = LastIODirectory::get();
	}

	expDialog.setExportSetting(s);
	expDialog.hideDataRange();

	expDialog.setWindowTitle(tr("Export contour figure to ESRI Shape files"));

	if (expDialog.exec() != QDialog::Accepted) {return;}

	s = expDialog.exportSetting();
	pInfo->setExportSetting(s);

	// start exporting.
	QProgressDialog dialog(this);
	dialog.setRange(s.startStep, s.endStep);
	dialog.setWindowTitle(tr("Export contour figure"));
	dialog.setLabelText(tr("Saving contour figure as ESRI Shape files..."));
	dialog.setFixedSize(300, 100);
	dialog.setModal(true);
	dialog.show();

	m_continuousSnapshotInProgress = true;

	int step = s.startStep;
	int fileIndex = 1;
	QDir outputFolder(s.folder);
	while (step <= s.endStep) {
		dialog.setValue(step);
		qApp->processEvents();
		if (dialog.wasCanceled()) {
			m_continuousSnapshotInProgress = false;
			return;
		}
		m_animationController->setCurrentStepIndex(step);
		QString prefixName = s.prefix;
		double time = m_projectData->mainfile()->postSolutionInfo()->currentTimeStep();
		bool ok = ew->exportContourFigureToShape(outputFolder.absoluteFilePath(prefixName), fileIndex, time, zoneName);
		if (! ok) {
			QMessageBox::critical(this, tr("Error"), tr("Error occured while saving."));
			m_continuousSnapshotInProgress = false;
			return;
		}
		step += s.skipRate;
		++ fileIndex;
	}
	m_continuousSnapshotInProgress = false;
}

void iRICMainWindow::exportStKML()
{
	static QString outputFileName;
	if (outputFileName == "") {
		QDir dir(LastIODirectory::get());
		outputFileName = dir.absoluteFilePath("output.kml");
	}

	if (m_solverConsoleWindow->isSolverRunning()) {
		warnSolverRunning();
		return;
	}
	SVKmlExportWindowInterface* ew = dynamic_cast<SVKmlExportWindowInterface*>(m_centralWidget->activeSubWindow()->widget());
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
		QMessageBox::warning(this, tr("Error"), tr("No contour is drawn now."));
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
		zoneName = zones.at(dialog.selectIndex());
	}
	if (! ew->checkKmlExportCondition(zoneName)) {return;}

	// show setting dialog
	PostDataExportDialog expDialog(this);

	PostSolutionInfo* pInfo = m_projectData->mainfile()->postSolutionInfo();
	expDialog.hideFormat();
	expDialog.hideDataRange();
	expDialog.setFileMode();
	expDialog.setTimeValues(pInfo->timeSteps()->timesteps());
	PostExportSetting s = pInfo->exportSetting();
	s.filename = outputFileName;
	expDialog.setExportSetting(s);
	expDialog.setWindowTitle(tr("Export Google Earth KML for street view"));

	if (expDialog.exec() != QDialog::Accepted) {return;}

	s = expDialog.exportSetting();
	pInfo->setExportSetting(s);

	outputFileName = s.filename;
	QFile mainKML(outputFileName);
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
	dialog.setWindowTitle(tr("Export Google Earth KML for street view"));
	dialog.setLabelText(tr("Saving KML files..."));
	dialog.setFixedSize(300, 100);
	dialog.setModal(true);
	dialog.show();

	m_continuousSnapshotInProgress = true;

	int step = s.startStep;
	int fileIndex = 1;
	while (step <= s.endStep) {
		dialog.setValue(step);
		qApp->processEvents();
		if (dialog.wasCanceled()) {
			m_continuousSnapshotInProgress = false;
			return;
		}
		m_animationController->setCurrentStepIndex(step);
		double time = m_projectData->mainfile()->postSolutionInfo()->currentTimeStep();
		bool ok = ew->exportKMLForTimestep(w, fileIndex, time, zoneName);
		if (! ok) {
			QMessageBox::critical(this, tr("Error"), tr("Error occured while saving."));
			m_continuousSnapshotInProgress = false;
			return;
		}
		step += s.skipRate;
		++ fileIndex;
	}

	ok = ew->exportKMLFooter(w, zoneName);

	w.writeEndElement();
	w.writeEndDocument();
	mainKML.close();

	m_continuousSnapshotInProgress = false;
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

void iRICMainWindow::checkCgnsStepsUpdate()
{
	if (m_projectData == nullptr) {return;}
	CursorChanger cursorChanger(QCursor(Qt::WaitCursor), this);
	m_projectData->mainfile()->postSolutionInfo()->checkCgnsStepsUpdate();
}

void iRICMainWindow::importMeasuredData()
{
	m_projectData->mainfile()->addMeasuredData();
	updatePostActionStatus();
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
		pre->projectDataItem()->loadFromProjectMainFile(tmpNode);
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
	// write setting about PreProcessor
	writer.writeStartElement("PreProcessorWindow");
	// delegate to PreProcessorWindowProjectDataItem
	PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*>(m_preProcessorWindow);
	pre->projectDataItem()->saveToProjectMainFile(writer);
	writer.writeEndElement();

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
	return pre->projectDataItem()->containedFiles();
}

void iRICMainWindow::loadFromCgnsFile(const int fn)
{
	PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*>(m_preProcessorWindow);
	pre->projectDataItem()->loadFromCgnsFile(fn);
}

void iRICMainWindow::saveToCgnsFile(const int fn)
{
	PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*>(m_preProcessorWindow);
	pre->projectDataItem()->saveToCgnsFile(fn);
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

void iRICMainWindow::clearResults()
{
	m_solverConsoleWindow->clear();
}

bool iRICMainWindow::clearResultsIfGridIsEdited()
{
	PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*>(m_preProcessorWindow);
	bool gridEdited = pre->projectDataItem()->isGridEdited();
	bool hasResult = m_projectData->mainfile()->postSolutionInfo()->hasResults();
	if (gridEdited && hasResult) {
		// grid is edited, and the CGNS has calculation result.
		int ret = QMessageBox::warning(m_preProcessorWindow, tr("Warning"), tr("The grids are edited. When you save, the calculation result is discarded."), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
		if (ret == QMessageBox::Cancel) {return false;}
		m_projectData->mainfile()->clearResults();
	}
	return true;
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

bool iRICMainWindow::isPostOnlyMode() const
{
	if (m_projectData == 0) {return false;}
	return m_projectData->isPostOnlyMode();
}
