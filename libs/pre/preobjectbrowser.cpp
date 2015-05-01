#include "preobjectbrowser.h"
#include "preobjectbrowserview.h"

#include <QItemSelection>
#include <QItemSelectionRange>
#include <QtGui/QIcon>
#include <QtGui/QStandardItemModel>

void PreObjectBrowser::init()
{
	// window properties
	setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);
	setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	setWindowTitle(tr("Object Browser"));

	// build tree
	m_view = new PreObjectBrowserView(this);
	m_view->setHeaderHidden(true);

	setWidget(m_view);
}

void PreObjectBrowser::setModel(QStandardItemModel* model)
{
	m_view->setModel(model);
	m_view->select(model->index(0, 0));
}

void PreObjectBrowser::expandAll()
{
	m_view->expandAll();
}

