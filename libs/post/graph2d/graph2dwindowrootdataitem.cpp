#include "graph2dwindowdatamodel.h"
#include "graph2dwindowrootdataitem.h"
#include "graph2dwindowview.h"

#include <guibase/objectbrowserview.h>
#include <guicore/datamodel/vtkgraphicsview.h>
#include <guicore/project/projectdata.h>
#include <misc/iricundostack.h>
#include <misc/xmlsupport.h>

#include <QStandardItemModel>
#include <QUndoCommand>
#include <QXmlStreamWriter>

#include <QVTKWidget.h>
#include <vtkRenderWindow.h>

Graph2dWindowRootDataItem::Graph2dWindowRootDataItem(QMainWindow* window, ProjectDataItem* parent)
	: Graph2dWindowDataItem(parent)
{
	// set preprocessor window.
	m_mainWindow = window;
	// set data model.
	m_dataModel = dynamic_cast<Graph2dWindowDataModel*>(parent);

//	updateZDepthRangeItemCount();
//	// update item map initially.
//	updateItemMap();
}

Graph2dWindowRootDataItem::~Graph2dWindowRootDataItem()
{

}

void Graph2dWindowRootDataItem::updateItemMap()
{
	m_itemMap.clear();
	innerUpdateItemMap(m_itemMap);
}

void Graph2dWindowRootDataItem::handleItemChange(QStandardItem* changedItem)
{
	Graph2dWindowDataItem* dataItem = modelItemFromItem(changedItem);
	if (dataItem == nullptr) {return;}
	dataItem->handleStandardItemChange();
}

void Graph2dWindowRootDataItem::handleItemDoubleClick(QStandardItem* clickedItem)
{
	Graph2dWindowDataItem* dataItem = modelItemFromItem(clickedItem);
	if (dataItem == nullptr) {return;}
	dataItem->handleStandardItemDoubleClicked();
}

void Graph2dWindowRootDataItem::deleteItem(QStandardItem* item)
{
	Graph2dWindowDataItem* dataItem = modelItemFromItem(item);
	// delete the item.
	delete dataItem;
	// rebuild m_itemMap.
	updateItemMap();
	// render graphics view.
	dataModel()->view()->replot();

	// delete operation is undo-able. clear the undo stack.
	iRICUndoStack::instance().clear();
}

class Graph2dWindowRootDataItemMoveUpCommand : public QUndoCommand
{
public:
	Graph2dWindowRootDataItemMoveUpCommand(Graph2dWindowDataItem* item, ObjectBrowserView* view)
		: QUndoCommand(QObject::tr("Move up item")) {
		m_item = item;
		m_view = view;
	}
	void redo() {
		m_view->setCommandExecution(true);
		m_item->moveUp();
		m_view->select(m_item->standardItem()->index());
		m_view->setCommandExecution(false);
	}
	void undo() {
		m_view->setCommandExecution(true);
		m_item->moveDown();
		m_view->select(m_item->standardItem()->index());
		m_view->setCommandExecution(false);
	}
private:
	Graph2dWindowDataItem* m_item;
	ObjectBrowserView* m_view;
};

void Graph2dWindowRootDataItem::moveUpItem(QStandardItem* item)
{
	Graph2dWindowDataItem* dataItem = modelItemFromItem(item);
	// move up the item.
	if (dataItem) {
		iRICUndoStack::instance().push(new Graph2dWindowRootDataItemMoveUpCommand(dataItem, m_dataModel->objectBrowserView()));
	}
}

class Graph2dWindowRootDataItemMoveDownCommand : public QUndoCommand
{
public:
	Graph2dWindowRootDataItemMoveDownCommand(Graph2dWindowDataItem* item, ObjectBrowserView* view)
		: QUndoCommand(QObject::tr("Move down item")) {
		m_item = item;
		m_view = view;
	}
	void redo() {
		m_view->setCommandExecution(true);
		m_item->moveDown();
		m_view->select(m_item->standardItem()->index());
		m_view->setCommandExecution(false);
	}
	void undo() {
		m_view->setCommandExecution(true);
		m_item->moveUp();
		m_view->select(m_item->standardItem()->index());
		m_view->setCommandExecution(false);
	}
private:
	Graph2dWindowDataItem* m_item;
	ObjectBrowserView* m_view;
};

void Graph2dWindowRootDataItem::moveDownItem(QStandardItem* item)
{
	Graph2dWindowDataItem* dataItem = modelItemFromItem(item);
	// move down the item.
	if (dataItem) {
		iRICUndoStack::instance().push(new Graph2dWindowRootDataItemMoveDownCommand(dataItem, m_dataModel->objectBrowserView()));
	}
}

Graph2dWindowDataItem* Graph2dWindowRootDataItem::modelItemFromItem(QStandardItem* item)
{
	return m_itemMap.value(item, nullptr);
}

void Graph2dWindowRootDataItem::loadFromCgnsFile(const int fn)
{
	Graph2dWindowDataItem::loadFromCgnsFile(fn);
	updateItemMap();
}

void Graph2dWindowRootDataItem::closeCgnsFile()
{
	Graph2dWindowDataItem::closeCgnsFile();
	updateItemMap();
}
