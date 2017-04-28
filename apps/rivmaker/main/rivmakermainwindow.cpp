#include "data/project/project.h"
#include "rivmakermainwindow.h"
#include "window/viewwindowi.h"
#include "../data/crosssections/crosssections.h"
#include "../dialogs/aboutdialog.h"
#include "../dialogs/mousehelpdialog.h"
#include "../io/sacguiimporter.h"
#include "../io/rivexporter.h"
#include "../io/polyline/polylineimporter.h"
#include "../window/crosssection/crosssectionwindow.h"

#include "ui_rivmakermainwindow.h"
#include "private/rivmakermainwindow_impl.h"

#include <QCloseEvent>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMessageBox>
#include <QPainter>

namespace {

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
	pw->showMaximized();
	auto vw = ui->centralwidget->addSubWindow(&(impl->m_verticalCrossSectionWindow));
	vw->setWindowIcon(impl->m_verticalCrossSectionWindow.windowIcon());
	vw->hide();

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
	if (impl->m_project != nullptr) {
		int ret = QMessageBox::warning(this, tr("Warning"), tr("All the data in the project is discarded. Are you sure?"), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
		if (ret == QMessageBox::Cancel) {
			return;
		}
	}

	deleteProject();
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
}

void RivmakerMainWindow::importElevation()
{
	impl->m_preProcessorWindow.importElevation();
	impl->m_preProcessorWindow.fit();
}

void RivmakerMainWindow::importWaterSurfaceElevation()
{
	impl->m_preProcessorWindow.importWaterSurfaceElevation();
	impl->m_preProcessorWindow.fit();
}

void RivmakerMainWindow::importSACGUIFile()
{
	std::vector<CrossSection*> newCrossSections;
	SACGUIImporter::importData(impl->m_project, &newCrossSections, this);
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
	RivExporter::exportData(*(impl->m_project), this);
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

void RivmakerMainWindow::zoomOut()
{
	auto w = currentViewWindow();
	if (w == nullptr) {return;}

	w->zoomOut();
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
	w->setProject(impl->m_project);

	connect(impl->m_project, SIGNAL(updated()), w, SLOT(updateView()));
	auto cw = dynamic_cast<QMdiArea*> (centralWidget());
	auto subW = cw->addSubWindow(w);

	subW->setWindowIcon(w->windowIcon());
	subW->show();
	subW->setFocus();

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

void RivmakerMainWindow::closeEvent(QCloseEvent *e)
{
	int ret = QMessageBox::warning(this, tr("Warning"), tr("Are you sure you want to exit Rivmaker?"), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
	if (ret == QMessageBox::Cancel) {
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
