#include "objectbrowser.h"

ObjectBrowser::ObjectBrowser(QWidget* parent) :
	QDockWidget {parent}
{
	setFeatures(QDockWidget::DockWidgetMovable);
	setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	setWindowTitle(ObjectBrowser::tr("Object Browser"));
}

ObjectBrowser::~ObjectBrowser()
{}
