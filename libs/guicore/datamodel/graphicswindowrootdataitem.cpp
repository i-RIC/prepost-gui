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

namespace {

class MoveUpCommand : public QUndoCommand
{
public:
	MoveUpCommand(GraphicsWindowDataItem* item, ObjectBrowserView* view) :
		QUndoCommand(QObject::tr("Move up item")),
		m_item {item},
		m_view {view}
	{}
	void redo() override {
		m_view->setCommandExecution(true);
		m_item->moveUp();
		m_view->select(m_item->standardItem()->index());
		m_view->setCommandExecution(false);
	}
	void undo() override {
		m_view->setCommandExecution(true);
		m_item->moveDown();
		m_view->select(m_item->standardItem()->index());
		m_view->setCommandExecution(false);
	}

private:
	GraphicsWindowDataItem* m_item;
	ObjectBrowserView* m_view;
};

class MoveDownCommand : public QUndoCommand
{
public:
	MoveDownCommand(GraphicsWindowDataItem* item, ObjectBrowserView* view) :
		QUndoCommand(QObject::tr("Move down item")),
		m_item {item},
		m_view {view}
	{}
	void redo() override {
		m_view->setCommandExecution(true);
		m_item->moveDown();
		m_view->select(m_item->standardItem()->index());
		m_view->setCommandExecution(false);
	}
	void undo() override {
		m_view->setCommandExecution(true);
		m_item->moveUp();
		m_view->select(m_item->standardItem()->index());
		m_view->setCommandExecution(false);
	}

private:
	GraphicsWindowDataItem* m_item;
	ObjectBrowserView* m_view;
};

} // namespace

GraphicsWindowRootDataItem::GraphicsWindowRootDataItem(QMainWindow* window, ProjectDataItem* parent) :
	GraphicsWindowDataItem(parent),
	m_mainWindow {window},
	m_dataModel {dynamic_cast<GraphicsWindowDataModel*> (parent)}
{}

GraphicsWindowRootDataItem::~GraphicsWindowRootDataItem()
{}

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

void GraphicsWindowRootDataItem::moveUpItem(QStandardItem* item)
{
	GraphicsWindowDataItem* dataItem = modelItemFromItem(item);
	if (dataItem == nullptr) {return;}

	iRICUndoStack::instance().push(new MoveUpCommand(dataItem, m_dataModel->objectBrowserView()));
}

void GraphicsWindowRootDataItem::moveDownItem(QStandardItem* item)
{
	GraphicsWindowDataItem* dataItem = modelItemFromItem(item);
	if (dataItem == nullptr) {return;}

	iRICUndoStack::instance().push(new MoveDownCommand(dataItem, m_dataModel->objectBrowserView()));
}

void GraphicsWindowRootDataItem::deleteItem(QStandardItem* item)
{
	GraphicsWindowDataItem* dataItem = modelItemFromItem(item);
	if (dataItem == nullptr) {return;}

	delete dataItem;
	updateItemMap();
	dataModel()->graphicsView()->GetRenderWindow()->Render();
}

void GraphicsWindowRootDataItem::updateItemMap()
{
	m_itemMap.clear();
	innerUpdateItemMap(m_itemMap);
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

bool GraphicsWindowRootDataItem::isAncientChecked() const
{
	return true;
}

GraphicsWindowDataModel* GraphicsWindowRootDataItem::dataModel() const
{
	return m_dataModel;
}

void GraphicsWindowRootDataItem::doLoadFromProjectMainFile(const QDomNode&)
{}

void GraphicsWindowRootDataItem::doSaveToProjectMainFile(QXmlStreamWriter&)
{}
