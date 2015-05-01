#include "post2dbirdeyeobjectbrowser.h"
#include "post2dbirdeyeobjectbrowserview.h"

#include <QItemSelection>
#include <QItemSelectionRange>
#include <QtGui/QIcon>
#include <QtGui/QStandardItemModel>

void Post2dBirdEyeObjectBrowser::init()
{
	// window properties
	setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);
	setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	setWindowTitle(tr("Object Browser"));

	// build tree
	m_view = new Post2dBirdEyeObjectBrowserView(this);
	m_view->setHeaderHidden(true);

	setWidget(m_view);
}

void Post2dBirdEyeObjectBrowser::setModel(QStandardItemModel* model)
{
	m_view->setModel(model);
}

void Post2dBirdEyeObjectBrowser::expandAll()
{
	m_view->expandAll();
}

