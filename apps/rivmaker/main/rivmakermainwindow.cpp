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

namespace {

} // namespace

RivmakerMainWindow::Impl::Impl(RivmakerMainWindow* w) :
	m_preProcessorWindow {w},
	m_verticalCrossSectionWindow {w},
	m_project {nullptr}
{
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
	ui->setupUi(this);
	setupConnections();

	auto pw = ui->centralwidget->addSubWindow(&(impl->m_preProcessorWindow));
	pw->setWindowIcon(impl->m_preProcessorWindow.windowIcon());
	pw->showMaximized();
	auto vw = ui->centralwidget->addSubWindow(&(impl->m_verticalCrossSectionWindow));
	vw->setWindowIcon(impl->m_verticalCrossSectionWindow.windowIcon());
	vw->hide();

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
	deleteProject();
	impl->m_project = new Project();
	impl->m_preProcessorWindow.setProject(impl->m_project);
	impl->m_verticalCrossSectionWindow.setProject(impl->m_project);

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

void RivmakerMainWindow::viewToggleToolBar(bool visible)
{
	ui->toolBar->setVisible(visible);
}

void RivmakerMainWindow::viewToggleStatusBar(bool visible)
{
	ui->statusbar->setVisible(visible);
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
	for (int i = 3; i < currentActions.size(); ++i) {
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
