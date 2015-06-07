#include "graphicswindowdatamodel.h"
#include "graphicswindowrootdataitem.h"
#include "vtkgraphicsview.h"

#include <guibase/objectbrowserview.h>
#include <misc/iricundostack.h>
#include <misc/xmlsupport.h>

#include <QStandardItemModel>
#include <QUndoCommand>
#include <QXmlStreamWriter>

#include <QVTKWidget.h>
#include <vtkRenderWindow.h>

GraphicsWindowRootDataItem::GraphicsWindowRootDataItem(QMainWindow* window, ProjectDataItem* parent)
	: GraphicsWindowDataItem(parent)
{
	// set preprocessor window.
	m_mainWindow = window;
	// set data model.
	m_dataModel = dynamic_cast<GraphicsWindowDataModel*>(parent);
}

GraphicsWindowRootDataItem::~GraphicsWindowRootDataItem()
{

}

void GraphicsWindowRootDataItem::updateItemMap()
{
	m_itemMap.clear();
	innerUpdateItemMap(m_itemMap);
}

void GraphicsWindowRootDataItem::handleItemChange(QStandardItem* changedItem)
{
	GraphicsWindowDataItem* dataItem = modelItemFromItem(changedItem);
	if (dataItem == nullptr) {return;}
	dataItem->handleStandardItemChange();
}

void GraphicsWindowRootDataItem::handleItemClick(QStandardItem* clickedItem)
{
	GraphicsWindowDataItem* dataItem = modelItemFromItem(clickedItem);
	if (dataItem == nullptr) {return;}
	dataItem->handleStandardItemClicked();
}

void GraphicsWindowRootDataItem::handleItemDoubleClick(QStandardItem* clickedItem)
{
	GraphicsWindowDataItem* dataItem = modelItemFromItem(clickedItem);
	if (dataItem == nullptr) {return;}
	dataItem->handleStandardItemDoubleClicked();
}

void GraphicsWindowRootDataItem::deleteItem(QStandardItem* item)
{
	GraphicsWindowDataItem* dataItem = modelItemFromItem(item);
	// delete the item.
	delete dataItem;
	// rebuild m_itemMap.
	updateItemMap();
	// render graphics view.
	dataModel()->graphicsView()->GetRenderWindow()->Render();
}

class GraphicsWindowRootDataItemMoveUpCommand : public QUndoCommand
{
public:
	GraphicsWindowRootDataItemMoveUpCommand(GraphicsWindowDataItem* item, ObjectBrowserView* view)
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
	GraphicsWindowDataItem* m_item;
	ObjectBrowserView* m_view;
};

void GraphicsWindowRootDataItem::moveUpItem(QStandardItem* item)
{
	GraphicsWindowDataItem* dataItem = modelItemFromItem(item);
	// move up the item.
	if (dataItem) {
		iRICUndoStack::instance().push(new GraphicsWindowRootDataItemMoveUpCommand(dataItem, m_dataModel->objectBrowserView()));
	}
}

class GraphicsWindowRootDataItemMoveDownCommand : public QUndoCommand
{
public:
	GraphicsWindowRootDataItemMoveDownCommand(GraphicsWindowDataItem* item, ObjectBrowserView* view)
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
	GraphicsWindowDataItem* m_item;
	ObjectBrowserView* m_view;
};

void GraphicsWindowRootDataItem::moveDownItem(QStandardItem* item)
{
	GraphicsWindowDataItem* dataItem = modelItemFromItem(item);
	// move down the item.
	if (dataItem) {
		iRICUndoStack::instance().push(new GraphicsWindowRootDataItemMoveDownCommand(dataItem, m_dataModel->objectBrowserView()));
	}
}

GraphicsWindowDataItem* GraphicsWindowRootDataItem::modelItemFromItem(QStandardItem* item)
{
	return m_itemMap.value(item, nullptr);
}

void GraphicsWindowRootDataItem::loadFromCgnsFile(const int fn)
{
	GraphicsWindowDataItem::loadFromCgnsFile(fn);
	updateItemMap();
}

void GraphicsWindowRootDataItem::closeCgnsFile()
{
	GraphicsWindowDataItem::closeCgnsFile();
	updateItemMap();
}
