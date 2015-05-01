#include "propertybrowser.h"
#include "../datamodel/propertybrowserview.h"

PropertyBrowser::PropertyBrowser(QWidget* parent)
	: QDockWidget(parent)
{
	// window properties
	setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	setWindowTitle(tr("Attribute Browser"));

	m_view = new PropertyBrowserView(this);
	setWidget(m_view);
	m_view->resetForVertex(true);
}

PropertyBrowser::~PropertyBrowser()
{

}

void PropertyBrowser::hideEvent(QHideEvent*)
{
	m_view->resetAttributes();
}
