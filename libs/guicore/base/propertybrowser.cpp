#include "../datamodel/propertybrowserview.h"
#include "propertybrowser.h"

PropertyBrowser::PropertyBrowser(QWidget* parent) :
	QDockWidget {parent}
{
	// window properties
	setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	setWindowTitle(PropertyBrowser::tr("Attribute Browser"));

	m_view = new PropertyBrowserView(this);
	setWidget(m_view);
	m_view->resetForVertex(true);
}

PropertyBrowser::~PropertyBrowser()
{}

PropertyBrowserView* PropertyBrowser::view() const
{
	return m_view;
}

void PropertyBrowser::hideEvent(QHideEvent*)
{
	m_view->resetAttributes();
}
