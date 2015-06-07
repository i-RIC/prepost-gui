#include "post3dobjectbrowser.h"
#include "post3dobjectbrowserview.h"

#include <QItemSelection>
#include <QItemSelectionRange>
#include <QtGui/QIcon>
#include <QtGui/QStandardItemModel>

void Post3dObjectBrowser::init()
{
	// window properties
	setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);
	setWindowTitle(tr("Object Browser"));

	// build tree
	m_view = new Post3dObjectBrowserView(this);
	m_view->setHeaderHidden(true);

	setWidget(m_view);
}

void Post3dObjectBrowser::setModel(QStandardItemModel* model)
{
	m_view->setModel(model);
}

void Post3dObjectBrowser::expandAll()
{
	m_view->expandAll();
}

