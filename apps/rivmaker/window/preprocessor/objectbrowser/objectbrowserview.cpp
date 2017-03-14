#include "objectbrowserview.h"

ObjectBrowserView::ObjectBrowserView(QWidget* w) :
	QTreeView(w)
{
	setHeaderHidden(true);
}

ObjectBrowserView::~ObjectBrowserView()
{}

void ObjectBrowserView::select(const QModelIndex& index)
{
	QItemSelectionModel* sm = selectionModel();
	sm->clear();
	sm->setCurrentIndex(index, QItemSelectionModel::SelectCurrent);
}

void ObjectBrowserView::currentChanged(const QModelIndex& current, const QModelIndex& previous)
{
	if (current.isValid()) {emit itemSelected(current);}
}
