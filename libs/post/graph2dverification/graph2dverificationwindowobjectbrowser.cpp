#include "graph2dverificationwindowobjectbrowser.h"
#include "graph2dverificationwindowobjectbrowserview.h"

#include <QItemSelection>
#include <QItemSelectionRange>
#include <QtGui/QIcon>
#include <QtGui/QStandardItemModel>

void Graph2dVerificationWindowObjectBrowser::init()
{
	// window properties
	setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);
	setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	setWindowTitle(tr("Object Browser"));

	// build tree
	m_view = new Graph2dVerificationWindowObjectBrowserView(this);
	m_view->setHeaderHidden(true);

	setWidget(m_view);
}

void Graph2dVerificationWindowObjectBrowser::setModel(QStandardItemModel* model)
{
	m_view->setModel(model);
}

void Graph2dVerificationWindowObjectBrowser::expandAll()
{
	m_view->expandAll();
}

