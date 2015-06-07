#include "graph2dhybridwindowobjectbrowser.h"
#include "graph2dhybridwindowobjectbrowserview.h"

#include <QItemSelection>
#include <QItemSelectionRange>
#include <QtGui/QIcon>
#include <QtGui/QStandardItemModel>

void Graph2dHybridWindowObjectBrowser::init()
{
	// window properties
	setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);
	setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	setWindowTitle(tr("Object Browser"));

	// build tree
	m_view = new Graph2dHybridWindowObjectBrowserView(this);
	m_view->setHeaderHidden(true);

	setWidget(m_view);
}

void Graph2dHybridWindowObjectBrowser::setModel(QStandardItemModel* model)
{
	m_view->setModel(model);
}

void Graph2dHybridWindowObjectBrowser::expandAll()
{
	m_view->expandAll();
}

