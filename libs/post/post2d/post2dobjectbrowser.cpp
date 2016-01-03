#include "post2dobjectbrowser.h"
#include "post2dobjectbrowserview.h"

#include <QItemSelection>
#include <QItemSelectionRange>
#include <QtGui/QIcon>
#include <QtGui/QStandardItemModel>

Post2dObjectBrowser::Post2dObjectBrowser(QWidget* parent) :
	ObjectBrowser(parent)
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

Post2dObjectBrowser::~Post2dObjectBrowser()
{}

void Post2dObjectBrowser::setModel(QStandardItemModel* model)
{
	m_view->setModel(model);
}

Post2dObjectBrowserView* Post2dObjectBrowser::view() const
{
	return m_view;
}

void Post2dObjectBrowser::expandAll()
{
	m_view->expandAll();
}
