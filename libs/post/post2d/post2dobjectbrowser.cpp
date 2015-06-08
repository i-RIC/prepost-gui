#include "post2dobjectbrowser.h"
#include "post2dobjectbrowserview.h"

#include <QItemSelection>
#include <QItemSelectionRange>
#include <QtGui/QIcon>
#include <QtGui/QStandardItemModel>

void Post2dObjectBrowser::init()
{
	// window properties
	setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);
	setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	setWindowTitle(tr("Object Browser"));

	// build tree
	m_view = new Post2dObjectBrowserView(this);
	m_view->setHeaderHidden(true);

	setWidget(m_view);
}

void Post2dObjectBrowser::setModel(QStandardItemModel* model)
{
	m_view->setModel(model);
}

void Post2dObjectBrowser::expandAll()
{
	m_view->expandAll();
}

