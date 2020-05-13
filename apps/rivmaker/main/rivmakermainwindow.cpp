#include "data/project/project.h"
#include "rivmakermainwindow.h"
#include "window/viewwindowi.h"
#include "../data/crosssections/crosssections.h"
#include "../dialogs/aboutdialog.h"
#include "../dialogs/graphicssettingdialog.h"
#include "../dialogs/mappingsettingdialog.h"
#include "../dialogs/mousehelpdialog.h"
#include "../io/sacguiimporter.h"
#include "../io/rivexporter.h"
#include "../io/polyline/polylineimporter.h"
#include "../widgets/mousepositionwidget.h"
#include "../window/crosssection/crosssectionwindow.h"

#include "ui_rivmakermainwindow.h"
#include "private/rivmakermainwindow_impl.h"

#include <QCloseEvent>
#include <QFileDialog>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMessageBox>
#include <QPainter>
#include <QSettings>

namespace {

const int MAX_RECENT_PROJECTS = 10;

} // namespace

RivmakerMainWindow::Impl::Impl(RivmakerMainWindow* w) :
	m_preProcessorWindow {w},
	m_verticalCrossSectionWindow {w},
	m_windowsToolBar {w},
	m_windowActivationMapper {w},
	m_valueWidget {w},
	m_mousePositionWidget {w},
	m_project {nullptr}
{
	m_windowsToolBar.setWindowTitle(RivmakerMainWindow::tr("Window List Toolbar"));
	m_windowsToolBar.setAllowedAreas(Qt::AllToolBarAreas);
	m_windowsToolBar.setFloatable(false);
	m_windowsToolBar.setIconSize(QSize(16, 16));
}

RivmakerMainWindow::Impl::~Impl()
{
	delete m_project;
}

// public interfaces

RivmakerMainWindow::RivmakerMainWindow(QWidget *parent) :
	QMainWindow(parent),
	impl {new Impl {this}},
	ui(new Ui::RivmakerMainWindow)
{
	addToolBar(Qt::RightToolBarArea, &(impl->m_windowsToolBar));

	ui->setupUi(this);
	ui->statusbar->addPermanentWidget(&(impl->m_mousePositionWidget));
	ui->statusbar->addPermanentWidget(&(impl->m_valueWidget));

	setupConnections();

	auto pw = ui->centralwidget->addSubWindow(&(impl->m_preProcessorWindow));
	pw->setWindowIcon(impl->m_preProcessorWindow.windowIcon());

	auto vw = ui->centralwidget->addSubWindow(&(impl->m_verticalCrossSectionWindow));
	vw->setWindowIcon(impl->m_verticalCrossSectionWindow.windowIcon());

	vw->showMaximized();
	pw->showMaximized();

	updateWindowsToolBar();
	newProject();
}

RivmakerMainWindow::~RivmakerMainWindow()
{
	deleteProject();
	delete ui;
	delete impl;
}

void RivmakerMainWindow::newProject()
{
	if (! closeProject()) {return;}

	impl->m_project = new Project();
	impl->m_preProcessorWindow.setProject(impl->m_project);
	impl->m_verticalCrossSectionWindow.setProject(impl->m_project);
	impl->m_mousePositionWidget.setProject(impl->m_project);

	auto windows = dynamic_cast<QMdiArea*> (centralWidget())->subWindowList();
	for (auto w : windows) {
		auto csw = dynamic_cast<CrossSectionWindow*> (w->widget());
		if (csw == nullptr) {continue;}
		csw->setProject(impl->m_project);
	}
	updateWindowTitle();
}

void RivmakerMainWindow::openProject()
{
	bool ok = closeProject();
	if (! ok) {return;}

	QString filename = QFileDialog::getOpenFileName(this, tr("Open project file"), impl->m_lastFolder, tr("RivMaker project file (*.rpro)"));
	if (filename.isNull()) {
		newProject();
		return;
	}

	openProject(filename);
}

bool RivmakerMainWindow::saveProject()
{
	auto p = impl->m_project;
	if (p->filename().isNull()) {
		return saveProjectAs();
	}

	bool ok = p->save(p->filename());
	if (ok) {
		ui->statusbar->showMessage(tr("Project saved to %1.").arg(QDir::toNativeSeparators(p->filename())), 5000);
		updateRecentProjects(p->filename());
		return true;
	} else {
		QMessageBox::critical(this, tr("Error"), tr("Saving project file to %1 failed.").arg(QDir::toNativeSeparators(p->filename())));
		return false;
	}
}

bool RivmakerMainWindow::saveProjectAs()
{
	auto p = impl->m_project;
	QString filename = QFileDialog::getSaveFileName(this, tr("Save project file"), impl->m_lastFolder, tr("RivMaker project file (*.rpro)"));
	if (filename.isNull()) {return false;}

	bool ok = p->save(filename);
	if (ok) {
		updateRecentProjects(p->filename());
		ui->statusbar->showMessage(tr("Project saved to %1.").arg(QDir::toNativeSeparators(p->filename())), 5000);
		updateWindowTitle();
		return true;
	} else {
		QMessageBox::critical(this, tr("Error"), tr("Saving project file to %1 failed.").arg(QDir::toNativeSeparators(p->filename())));
		return false;
	}
}

void RivmakerMainWindow::openRecentProject()
{
	QAction* action = qobject_cast<QAction*>(sender());
	if (action) {
		openProject(action->data().toString());
	}
}

void RivmakerMainWindow::importElevation()
{
	impl->m_preProcessorWindow.importElevation();
	impl->m_project->updatePointsAutoSize();
	impl->m_preProcessorWindow.fit();
}

void RivmakerMainWindow::importWaterSurfaceElevation()
{
	impl->m_preProcessorWindow.importWaterSurfaceElevation();
	impl->m_project->updatePointsAutoSize();
	impl->m_preProcessorWindow.fit();
}

void RivmakerMainWindow::importSACGUIFile()
{
	std::vector<CrossSection*> newCrossSections;
	SACGUIImporter::importData(impl->m_project, &newCrossSections, this);
	impl->m_project->updatePointsAutoSize();
	impl->m_preProcessorWindow.setCrossSections(newCrossSections);
	impl->m_preProcessorWindow.fit();
}

void RivmakerMainWindow::importBaseLine()
{
	impl->m_preProcessorWindow.importBaseLine();
}

void RivmakerMainWindow::importCrossSectionLines()
{
	impl->m_preProcessorWindow.importCrossSections();
}

void RivmakerMainWindow::exportBaseLine()
{
	impl->m_preProcessorWindow.exportBaseLine();
}

void RivmakerMainWindow::exportCrossSectionLines()
{
	impl->m_preProcessorWindow.exportCrossSections();
}

void RivmakerMainWindow::exportRiverSurveyData()
{
	if (impl->m_project->crossSections().crossSectionVector().size() == 0) {
		QMessageBox::warning(this, tr("Error"), tr("No data to export exists"));
		return;
	}
	impl->m_rivExporter.exportData(impl->m_project, this);
}

void RivmakerMainWindow::exportRiverSurveyCsvData()
{
	if (impl->m_project->crossSections().crossSectionVector().size() == 0) {
		QMessageBox::warning(this, tr("Error"), tr("No data to export exists"));
		return;
	}
	impl->m_rivCsvExporter.exportData(impl->m_project, this);
}

void RivmakerMainWindow::exportWaterSurfaceElevationData()
{
	if (impl->m_project->crossSections().crossSectionVector().size() == 0) {
		QMessageBox::warning(this, tr("Error"), tr("No data to export exists"));
		return;
	}
	impl->m_rivExporter.exportCsvData(impl->m_project, this);
}

void RivmakerMainWindow::fit()
{
	auto w = currentViewWindow();
	if (w == nullptr) {return;}

	w->fit();
}

void RivmakerMainWindow::resetRotation()
{
	auto w = currentViewWindow();
	if (w == nullptr) {return;}

	w->resetRotation();
}

void RivmakerMainWindow::rotate90()
{
	auto w = currentViewWindow();
	if (w == nullptr) {return;}

	w->rotate90();
}

void RivmakerMainWindow::zoomIn()
{
	auto w = currentViewWindow();
	if (w == nullptr) {return;}

	w->zoomIn();
}

void RivmakerMainWindow::zoomInX()
{
	auto w = currentViewWindow();
	if (w == nullptr) {return;}

	w->zoomInX();
}

void RivmakerMainWindow::zoomInY()
{
	auto w = currentViewWindow();
	if (w == nullptr) {return;}

	w->zoomInY();
}

void RivmakerMainWindow::zoomOut()
{
	auto w = currentViewWindow();
	if (w == nullptr) {return;}

	w->zoomOut();
}

void RivmakerMainWindow::zoomOutX()
{
	auto w = currentViewWindow();
	if (w == nullptr) {return;}

	w->zoomOutX();
}

void RivmakerMainWindow::zoomOutY()
{
	auto w = currentViewWindow();
	if (w == nullptr) {return;}

	w->zoomOutY();
}

void RivmakerMainWindow::focusPreProcessorWindow()
{
	auto pw = impl->m_preProcessorWindow.parentWidget();
	pw->show();
	pw->setFocus();
}

void RivmakerMainWindow::focusVerticalCrossSectionWindow()
{
	auto pw = impl->m_verticalCrossSectionWindow.parentWidget();
	pw->show();
	pw->setFocus();
}

void RivmakerMainWindow::openCrossSectionWindow()
{
	if (impl->m_project->crossSections().crossSectionVector().size() == 0) {
		QMessageBox::warning(this, tr("Warning"), tr("Cross Section Window can not be opened when no Cross Section is defined."));
		return;
	}

	auto w = new CrossSectionWindow(this);
	connect(w, SIGNAL(positionChangedForStatusBar(QPointF)), &(impl->m_mousePositionWidget), SLOT(updatePositionWithoutOffset(QPointF)));
	w->setProject(impl->m_project);

	connect(impl->m_project, SIGNAL(updated()), w, SLOT(updateView()));
	auto cw = dynamic_cast<QMdiArea*> (centralWidget());
	auto subW = cw->addSubWindow(w);

	subW->setWindowIcon(w->windowIcon());
	subW->show();
	subW->setFocus();
	w->fit();

	updateWindowsToolBar();
	connect(w, SIGNAL(destroyed(QObject*)), this, SLOT(updateWindowsToolBar()));
}

void RivmakerMainWindow::baseLineAddPoint()
{
	impl->m_preProcessorWindow.baseLineAddVertex();
}

void RivmakerMainWindow::baseLineRemovePoint()
{
	impl->m_preProcessorWindow.baseLineRemoveVertex();
}

void RivmakerMainWindow::baseLineEditCoordinates()
{
	impl->m_preProcessorWindow.baseLineEditCoordinates();
}

void RivmakerMainWindow::baseLineReverseDirection()
{
	impl->m_preProcessorWindow.baseLineReverseDirection();
}

void RivmakerMainWindow::baseLineDelete()
{
	impl->m_preProcessorWindow.baseLineDelete();
}

void RivmakerMainWindow::crossSectionAdd()
{
	impl->m_preProcessorWindow.crossSectionAdd();
}

void RivmakerMainWindow::crossSectionEditCoordinates()
{
	impl->m_preProcessorWindow.crossSectionEditCoordinates();
}

void RivmakerMainWindow::crossSectionDelete()
{
	impl->m_preProcessorWindow.crossSectionDelete();
}

void RivmakerMainWindow::viewToggleMainToolBar(bool visible)
{
	ui->toolBar->setVisible(visible);
}

void RivmakerMainWindow::viewToggleWindowsToolBar(bool visible)
{
	impl->m_windowsToolBar.setVisible(visible);
}

void RivmakerMainWindow::viewToggleStatusBar(bool visible)
{
	ui->statusbar->setVisible(visible);
}

void RivmakerMainWindow::optionMappingSetting()
{
	MappingSettingDialog dialog(this);
	dialog.setProject(impl->m_project);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	impl->m_project->mapPointsToCrossSections();
	impl->m_project->emitUpdated();
}

void RivmakerMainWindow::optionGraphicsSetting()
{
	GraphicsSettingDialog dialog(this);
	dialog.exec();
}

void RivmakerMainWindow::helpMouseHint()
{
	static MouseHelpDialog* dialog = nullptr;
	if (dialog == nullptr) {
		dialog = new MouseHelpDialog(this);
	}
	dialog->show();
}

void RivmakerMainWindow::showAboutDialog()
{
	AboutDialog dialog(this);
	dialog.exec();
}

void RivmakerMainWindow::updateViewMenu()
{
	QMenu* vMenu = ui->viewMenu;
	auto currentActions = vMenu->actions();
	for (int i = 4; i < currentActions.size(); ++i) {
		auto a = currentActions.at(i);
		vMenu->removeAction(a);
	}
	auto& mw = impl->m_preProcessorWindow;
	QAction* mainWindowAction = new QAction(mw.windowIcon(), mw.windowTitle(), vMenu);
	vMenu->addAction(mainWindowAction);
	connect(mainWindowAction, SIGNAL(triggered()), this, SLOT(focusPreProcessorWindow()));

	auto& vw = impl->m_verticalCrossSectionWindow;
	QAction* vWindowAction = new QAction(vw.windowIcon(), vw.windowTitle(), vMenu);
	vMenu->addAction(vWindowAction);
	connect(vWindowAction, SIGNAL(triggered()), this, SLOT(focusVerticalCrossSectionWindow()));

	auto windows = dynamic_cast<QMdiArea*> (centralWidget())->subWindowList();
	for (auto w : windows) {
		auto csw = dynamic_cast<CrossSectionWindow*> (w->widget());
		if (csw == nullptr) {continue;}

		QAction* a = new QAction(csw->windowIcon(), csw->windowTitle(), vMenu);
		vMenu->addAction(a);
		connect(a, SIGNAL(triggered()), w, SLOT(setFocus()));
	}
}

void RivmakerMainWindow::updateWindowsToolBar()
{
	for (QAction* a : impl->m_windowsToolBar.actions()) {
		impl->m_windowActivationMapper.removeMappings(a);
	}
	impl->m_windowsToolBar.clear();

	QPixmap shortcutPixmap(":/images/iconShortcut.png");
	QSize iconSize = impl->m_windowsToolBar.iconSize();

	QMdiArea* mdiArea = dynamic_cast<QMdiArea*>(centralWidget());
	QList<QMdiSubWindow*> windowList = mdiArea->subWindowList();
	QPainter painter;
	for (QMdiSubWindow* w : windowList) {
		QAction* a = new QAction(w->windowTitle(), &(impl->m_windowsToolBar));
		QPixmap iconWithShortcut(w->windowIcon().pixmap(iconSize));
		painter.begin(&iconWithShortcut);
		painter.drawPixmap(0, 0, iconSize.width(), iconSize.height(), shortcutPixmap);
		painter.end();
		a->setIcon(QIcon(iconWithShortcut));

		impl->m_windowsToolBar.addAction(a);
		connect(a, SIGNAL(triggered()), &(impl->m_windowActivationMapper), SLOT(map()));
		impl->m_windowActivationMapper.setMapping(a, w);
	}
}

void RivmakerMainWindow::activateWindow(QWidget* w)
{
	w->show();
	w->setFocus();
}

void RivmakerMainWindow::setupRecentProjectsMenu()
{
	QMenu* menu = ui->recentProjectsMenu;
	for (auto a : menu->actions()) {
		delete a;
	}
	menu->clear();

	QSettings setting("iRIC Organization", "RivMaker");
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

void RivmakerMainWindow::openProject(const QString& filename)
{
	bool ok = closeProject();
	if (! ok) {return;}

	Project* newP = new Project();
	ok = newP->load(filename);
	if (! ok) {
		QMessageBox::critical(this, tr("Error"), tr("Opening project file failed."));
		delete newP;

		newProject();
		return;
	}
	impl->m_project = newP;

	impl->m_preProcessorWindow.setProject(impl->m_project);
	impl->m_verticalCrossSectionWindow.setProject(impl->m_project);
	impl->m_mousePositionWidget.setProject(impl->m_project);

	auto windows = dynamic_cast<QMdiArea*> (centralWidget())->subWindowList();
	for (auto w : windows) {
		auto csw = dynamic_cast<CrossSectionWindow*> (w->widget());
		if (csw == nullptr) {continue;}
		csw->setProject(impl->m_project);
	}

	QFileInfo finfo(filename);
	impl->m_lastFolder = finfo.absolutePath();
	updateRecentProjects(filename);
	updateWindowTitle();
}

bool RivmakerMainWindow::closeProject()
{
	if (impl->m_project == nullptr) {return true;}

	if (impl->m_project->isModified()) {
		int ret = QMessageBox::warning(this, tr("Warning"), tr("The project data is modified. Do you want to save?"), QMessageBox::Cancel | QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
		if (ret == QMessageBox::Cancel) {
			return false;
		} else if (ret == QMessageBox::Yes) {
			bool ok = saveProject();
			if (! ok) {return false;}
		}
	}

	deleteProject();

	return true;
}

void RivmakerMainWindow::updateWindowTitle()
{
	if (impl->m_project == nullptr || impl->m_project->filename().isNull()) {
		setWindowTitle("RivMaker 1.0");
	} else {
		QString title("%1 - RivMaker 1.0");
		QFileInfo finfo(impl->m_project->filename());
		setWindowTitle(title.arg(finfo.fileName()));
	}
}


void RivmakerMainWindow::updateRecentProjects(const QString& filename)
{
	QSettings setting("iRIC Organization", "RivMaker");
	QStringList recentProjects = setting.value("general/recentprojects", QStringList()).toStringList();
	recentProjects.removeAll(filename);
	recentProjects.prepend(filename);
	while (recentProjects.size() > MAX_RECENT_PROJECTS) {
		recentProjects.removeLast();
	}
	setting.setValue("general/recentprojects", recentProjects);
}

void RivmakerMainWindow::removeFromRecentProjects(const QString& filename)
{
	QSettings setting("iRIC Organization", "RivMaker");
	QStringList recentProjects = setting.value("general/recentprojects", QStringList()).toStringList();
	recentProjects.removeAll(filename);
	while (recentProjects.size() > MAX_RECENT_PROJECTS) {
		recentProjects.removeLast();
	}
	setting.setValue("general/recentprojects", recentProjects);
}

void RivmakerMainWindow::closeEvent(QCloseEvent *e)
{
	bool ok = closeProject();
	if (! ok) {
		e->ignore();
		return;
	}
	e->accept();
}

void RivmakerMainWindow::deleteProject()
{
	delete impl->m_project;
	impl->m_project = nullptr;
}

ViewWindowI* RivmakerMainWindow::currentViewWindow() const
{
	auto mdiArea = dynamic_cast<QMdiArea*> (centralWidget());
	auto w = mdiArea->currentSubWindow()->widget();
	return dynamic_cast<ViewWindowI*> (w);
}
