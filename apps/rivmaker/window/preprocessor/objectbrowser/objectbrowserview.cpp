#include "objectbrowserview.h"

ObjectBrowserView::ObjectBrowserView(QWidget* w) :
	QTreeView(w)
{
	setHeaderHidden(true);
}

ObjectBrowserView::~ObjectBrowserView()
{}
