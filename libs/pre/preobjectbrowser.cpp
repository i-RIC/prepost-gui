#include "preobjectbrowser.h"
#include "preobjectbrowserview.h"

#include <QStandardItemModel>

PreObjectBrowser::PreObjectBrowser(QWidget* parent) :
	ObjectBrowser(parent),
	m_view {new PreObjectBrowserView(this)}
{
	// window properties
	setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);
	setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	setWindowTitle(PreObjectBrowser::tr("Object Browser"));
	setWidget(m_view);

	m_view->setHeaderHidden(true);
}

PreObjectBrowser::~PreObjectBrowser()
{}

void PreObjectBrowser::setModel(QStandardItemModel* model)
{
	m_view->setModel(model);
	m_view->select(model->index(0, 0));
}

PreObjectBrowserView* PreObjectBrowser::view() const
{
	return m_view;
}

void PreObjectBrowser::expandAll()
{
	m_view->expandAll();
}

