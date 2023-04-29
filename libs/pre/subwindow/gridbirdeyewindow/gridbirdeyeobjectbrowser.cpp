#include "gridbirdeyeobjectbrowser.h"
#include "gridbirdeyeobjectbrowserview.h"

GridBirdEyeObjectBrowser::GridBirdEyeObjectBrowser(QWidget* parent) :
	ObjectBrowser(parent)
{
	init();
}

GridBirdEyeObjectBrowser::~GridBirdEyeObjectBrowser()
{}

void GridBirdEyeObjectBrowser::setModel(QStandardItemModel* model)
{
	m_view->setModel(model);
}

GridBirdEyeObjectBrowserView* GridBirdEyeObjectBrowser::view() const
{
	return m_view;
}

void GridBirdEyeObjectBrowser::expandAll()
{
	m_view->expandAll();
}

void GridBirdEyeObjectBrowser::init()
{
	// window properties
	setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);
	setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	setWindowTitle(tr("Object Browser"));

	// build tree
	m_view = new GridBirdEyeObjectBrowserView(this);
	m_view->setHeaderHidden(true);

	setWidget(m_view);
}

