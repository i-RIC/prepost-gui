#include "objectbrowser/objectbrowser.h"
#include "objectbrowser/objectbrowserview.h"
#include "preprocessorwindow.h"
#include "private/preprocessorwindow_impl.h"

#include <QCloseEvent>

PreProcessorWindow::Impl::Impl(PreProcessorWindow* w) :
	m_objectBrowser {nullptr},
	m_view {w},
	m_model {w}
{
	m_model.setView(&m_view);

	m_objectBrowser = new ObjectBrowser(w);
	w->addDockWidget(Qt::LeftDockWidgetArea, m_objectBrowser);

	auto obView = new ObjectBrowserView(m_objectBrowser);
	m_objectBrowser->setWidget(obView);

	obView->setModel(m_model.standardItemModel());
	m_model.setObjectBrowserView(obView);

	w->setCentralWidget(&m_view);
}

PreProcessorWindow::Impl::~Impl()
{}

// public interfaces

PreProcessorWindow::PreProcessorWindow(QWidget *parent) :
	QMainWindow(parent),
	impl {new Impl {this}}
{
	setWindowTitle(tr("Main Window"));
	setWindowIcon(QIcon(":images/iconMain.png"));

	resize(600, 500);
}

PreProcessorWindow::~PreProcessorWindow()
{
	delete impl;
}

void PreProcessorWindow::setProject(Project* project)
{
	impl->m_model.setProject(project);
}

void PreProcessorWindow::fit()
{
	impl->m_view.fit();
}

void PreProcessorWindow::resetRotation()
{
	impl->m_view.resetRotation();
}

void PreProcessorWindow::rotate90()
{
	impl->m_view.rotate90();
}

void PreProcessorWindow::zoomIn()
{
	impl->m_view.zoomIn();
}

void PreProcessorWindow::zoomOut()
{
	impl->m_view.zoomOut();
}

void PreProcessorWindow::importElevation()
{
	impl->m_model.importElevation();
}

void PreProcessorWindow::importWaterSurfaceElevation()
{
	impl->m_model.importWaterSurfaceElevation();
}

void PreProcessorWindow::importBaseLine()
{
	impl->m_model.importBaseLine();
}

void PreProcessorWindow::exportBaseLine()
{
	impl->m_model.exportBaseLine();
}

void PreProcessorWindow::importCrossSections()
{
	impl->m_model.importCrossSections();
}

void PreProcessorWindow::exportCrossSections()
{
	impl->m_model.exportCrossSections();
}

void PreProcessorWindow::crossSectionAdd()
{
	impl->m_model.addCrossSection();
}

void PreProcessorWindow::crossSectionDelete()
{
	impl->m_model.deleteCrossSection();
}

void PreProcessorWindow::crossSectionEditCoordinates()
{
	impl->m_model.editCrossSectionCoordinates();
}

void PreProcessorWindow::baseLineAddVertex()
{
	impl->m_model.addBaseLineVertex();
}

void PreProcessorWindow::baseLineRemoveVertex()
{
	impl->m_model.removeBaseLineVertex();
}

void PreProcessorWindow::baseLineEditCoordinates()
{
	impl->m_model.editBaseLineCoordinates();
}

void PreProcessorWindow::baseLineReverseDirection()
{
	impl->m_model.reverseBaseLineDirection();
}

void PreProcessorWindow::baseLineDelete()
{
	impl->m_model.deleteBaseLine();
}

void PreProcessorWindow::closeEvent(QCloseEvent *e)
{
	parentWidget()->hide();
	e->ignore();
}
