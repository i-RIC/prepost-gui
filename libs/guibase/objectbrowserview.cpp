#include "objectbrowserview.h"

#include <misc/iricundostack.h>

#include <QAction>
#include <QIcon>
#include <QMessageBox>
#include <QMouseEvent>
#include <QStandardItemModel>
#include <QUndoCommand>

ObjectBrowserView::ObjectBrowserView(QWidget* parent) :
	QTreeView(parent)
{
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

	m_isPushing = false;
	m_commandExecution = false;

	m_addAction = new QAction(QIcon(":/libs/guibase/images/iconProperty.png"),tr("&Add..."), this);
	connect(m_addAction, SIGNAL(triggered()), this, SLOT(showAddForCurrentItem()));

	m_deleteAction = new QAction(QIcon(":/libs/guibase/images/iconDeleteItem.png"),tr("&Delete..."), this);
	connect(m_deleteAction, SIGNAL(triggered()), this, SLOT(deleteCurrentItem()));

	m_undoableDeleteAction = new QAction(QIcon(":/libs/guibase/images/iconDeleteItem.png"),tr("&Delete..."), this);
	connect(m_undoableDeleteAction, SIGNAL(triggered()), this, SLOT(undoableDeleteCurrentItem()));

	m_moveUpAction = new QAction(QIcon(":/libs/guibase/images/iconItemMoveUp.png"), tr("Move up"), this);
	connect(m_moveUpAction, SIGNAL(triggered()), this, SLOT(moveUpCurrentItem()));

	m_moveDownAction = new QAction(QIcon(":/libs/guibase/images/iconItemMoveDown.png"), tr("Move down"), this);
	connect(m_moveDownAction, SIGNAL(triggered()), this, SLOT(moveDownCurrentItem()));

	m_propertyAction = new QAction(tr("&Property..."), this);
	m_propertyAction->setIcon(QIcon(":/libs/guibase/images/iconProperty.png"));
	connect(m_propertyAction, SIGNAL(triggered()), this, SLOT(showPropertyForCurrentItem()));

	connect(this, SIGNAL(pressed(QModelIndex)), this, SLOT(handlePress(QModelIndex)));

	setSelectionMode(SingleSelection);
}

QAction* ObjectBrowserView::addAction() const
{
	return m_addAction;
}

QAction* ObjectBrowserView::deleteAction() const
{
	return m_deleteAction;
}

QAction* ObjectBrowserView::moveUpAction() const
{
	return m_moveUpAction;
}

QAction* ObjectBrowserView::moveDownAction() const
{
	return m_moveDownAction;
}

QAction* ObjectBrowserView::propertyAction() const
{
	return m_propertyAction;
}

QAction* ObjectBrowserView::undoableDeleteAction() const
{
	return m_undoableDeleteAction;
}

QSize ObjectBrowserView::sizeHint() const
{
	return QSize(250, 200);
}

void ObjectBrowserView::deleteCurrentItem()
{
	QModelIndex selected = *(selectedIndexes().begin());
	QStandardItemModel* itemModel = dynamic_cast<QStandardItemModel*>(model());
	QStandardItem* selectedItem = itemModel->itemFromIndex(selected);
	QString text = selectedItem->data(Qt::UserRole + 20).toString();

	if (text.isEmpty()) {
		text = tr("Are you sure you want to delete this item?");
	}
	if (QMessageBox::No == QMessageBox::warning(this, tr("Delete item"), tr(text.toStdString().c_str()), QMessageBox::Yes | QMessageBox::No, QMessageBox::No)) {
		return;
	}

	QStandardItem* parentItem = itemModel->itemFromIndex(selected.parent());
	QString qstr = parentItem->data(Qt::UserRole + 10).toString();
	if (qstr == "BACKGROUNDIMAGES") {
		emit requestDeleteImage(selected);
	} else if (qstr == "MEASUREDDATAS") {
		emit requestDeleteMeasuredData(selected);
	} else {
		emit requestDeleteItem(selected);
	}
}

void ObjectBrowserView::undoableDeleteCurrentItem()
{
	QModelIndex selected = *(selectedIndexes().begin());
	emit requestUndoableDeleteItem(selected);
}

void ObjectBrowserView::moveUpCurrentItem()
{
	QModelIndex selected = *(selectedIndexes().begin());
	QStandardItemModel* itemModel = dynamic_cast<QStandardItemModel*>(model());
	QStandardItem* parentItem = itemModel->itemFromIndex(selected.parent());
	QString qstr = parentItem->data(Qt::UserRole + 10).toString();
	if (qstr == "BACKGROUNDIMAGES") {
		emit requestMoveUpImage(selected);
	} else {
		emit requestMoveUpItem(selected);
	}
}

void ObjectBrowserView::moveDownCurrentItem()
{
	QModelIndex selected = *(selectedIndexes().begin());
	QStandardItemModel* itemModel = dynamic_cast<QStandardItemModel*>(model());
	QStandardItem* parentItem = itemModel->itemFromIndex(selected.parent());
	QString qstr = parentItem->data(Qt::UserRole + 10).toString();
	if (qstr == "BACKGROUNDIMAGES") {
		emit requestMoveDownImage(selected);
	} else {
		emit requestMoveDownItem(selected);
	}
}

void ObjectBrowserView::showPropertyForCurrentItem()
{
	QModelIndex selected = *(selectedIndexes().begin());
	emit requestShowPropertyDialog(selected);
}

void ObjectBrowserView::showAddForCurrentItem()
{
	QModelIndex selected = *(selectedIndexes().begin());
	emit requestShowAddDialog(selected);
}

void ObjectBrowserView::mousePressEvent(QMouseEvent* event)
{
	m_pressGlobalPos = event->globalPos();
	QTreeView::mousePressEvent(event);
}

void ObjectBrowserView::handlePress(const QModelIndex& index)
{
	emit pressed(index, m_pressGlobalPos);
}

void ObjectBrowserView::currentChanged(const QModelIndex& current, const QModelIndex& previous)
{
	if (previous.isValid()) {emit itemDeselected(previous);}
	if (current.isValid()) {emit itemSelected(current);}
	emit selectionChanged();
}

void ObjectBrowserView::select(const QModelIndex& newindex)
{
	QModelIndex oldindex;
	if (selectedIndexes().count() == 0) {
		oldindex = newindex;
	} else {
		oldindex = *(selectedIndexes().begin());
	}

	setCommandExecution(true);
	selectionModel()->clear();
	selectionModel()->setCurrentIndex(newindex, QItemSelectionModel::SelectCurrent);
	setCommandExecution(false);
}

void ObjectBrowserView::setCommandExecution(bool exec)
{
	m_commandExecution = exec;
}
