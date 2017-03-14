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
}

PreProcessorWindow::~PreProcessorWindow()
{
	delete impl;
}

void PreProcessorWindow::setProject(Project* project)
{
	impl->m_model.setProject(project);
}

void PreProcessorWindow::closeEvent(QCloseEvent *e)
{
	parentWidget()->hide();
	e->ignore();
}
