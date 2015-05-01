#include "graph2dscatteredwindowobjectbrowser.h"
#include "graph2dscatteredwindowobjectbrowserview.h"

#include <QItemSelection>
#include <QItemSelectionRange>
#include <QtGui/QIcon>
#include <QtGui/QStandardItemModel>

void Graph2dScatteredWindowObjectBrowser::init()
{
	// window properties
	setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);
	setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	setWindowTitle(tr("Object Browser"));

	// build tree
	m_view = new Graph2dScatteredWindowObjectBrowserView(this);
	m_view->setHeaderHidden(true);

	setWidget(m_view);
}

void Graph2dScatteredWindowObjectBrowser::setModel(QStandardItemModel* model)
{
	m_view->setModel(model);
}

void Graph2dScatteredWindowObjectBrowser::expandAll()
{
	m_view->expandAll();
}

