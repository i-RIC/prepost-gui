#include "objectbrowserview.h"

#include <misc/iricundostack.h>

#include <QAction>
#include <QIcon>
#include <QMessageBox>
#include <QMouseEvent>
#include <QStandardItemModel>
#include <QUndoCommand>

ObjectBrowserView::ObjectBrowserView(QWidget* parent)
	: QTreeView(parent)
{
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

	m_isPushing = false;
	m_commandExecution = false;
	m_deleteAction = new QAction(QIcon(":/libs/guibase/images/iconDeleteItem.png"),tr("&Delete..."), this);
	connect(m_deleteAction, SIGNAL(triggered()), this, SLOT(deleteCurrentItem()));
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

void ObjectBrowserView::deleteCurrentItem()
{
	if (QMessageBox::No == QMessageBox::warning(this, tr("Delete item"), tr("Are you sure you want to delete this item?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No)) {
		return;
	}
	QModelIndex selected = *(selectedIndexes().begin());
	QStandardItemModel* itemModel = dynamic_cast<QStandardItemModel*>(model());
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


void ObjectBrowserView::mousePressEvent(QMouseEvent* event)
{
	m_pressGlobalPos = event->globalPos();
	QTreeView::mousePressEvent(event);
}

void ObjectBrowserView::handlePress(const QModelIndex& index)
{
	emit pressed(index, m_pressGlobalPos);
}

/// Command to change selection in object browser
class ObjectBrowserViewCurrentChangeCommand : public QUndoCommand
{
public:
	ObjectBrowserViewCurrentChangeCommand(const QModelIndex& before, const QModelIndex& after, ObjectBrowserView* view)
		: QUndoCommand(QObject::tr("Selection Change")) {
		m_before = before;
		m_after = after;
		m_view = view;
	}
	void undo() {
		m_view->setCommandExecution(true);
		m_view->selectionModel()->clear();
		m_view->selectionModel()->setCurrentIndex(m_before, QItemSelectionModel::SelectCurrent);
		m_view->setCommandExecution(false);
	}
	void redo() {
		m_view->setCommandExecution(true);
		m_view->selectionModel()->clear();
		m_view->selectionModel()->setCurrentIndex(m_after, QItemSelectionModel::SelectCurrent);
		m_view->setCommandExecution(false);
	}
private:
	QModelIndex m_before;
	QModelIndex m_after;
	ObjectBrowserView* m_view;
};

void ObjectBrowserView::currentChanged(const QModelIndex& current, const QModelIndex& previous)
{
	/*
		if (! m_commandExecution){
			m_isPushing = true;
			iRICUndoStack::instance().push(new ObjectBrowserViewCurrentChangeCommand(previous, current, this));
			m_isPushing = false;
		}
	*/
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
	QUndoCommand* command = new ObjectBrowserViewCurrentChangeCommand(oldindex, newindex, this);
	command->redo();
	delete command;
}
