#define _USE_MATH_DEFINES
#include <cmath>

#include "graphicswindowdatamodel.h"
#include <misc/mathsupport.h>
#include <guibase/irictoolbar.h>
#include <misc/iricundostack.h>
#include "graphicswindowrootdataitem.h"
#include <guibase/objectbrowserview.h>
#include "vtkgraphicsview.h"
#include "../base/iricmainwindowinterface.h"
#include "../project/projectdata.h"

#include <vtkRenderWindow.h>
#include <vtkCamera.h>
#include <vtkRenderer.h>

#include <QTreeView>
#include <QGraphicsItemGroup>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QVector2D>
#include <QMenu>
#include <QApplication>
#include <QDialog>

GraphicsWindowDataModel::GraphicsWindowDataModel(QMainWindow* w, ProjectDataItem* parent)
	: GraphicsWindowSimpleDataModel(w, parent)
{
//	m_graphicsView = dynamic_cast<VTKGraphicsView*>(w->centralWidget());
	m_rootDataItem = nullptr;
	m_rightClickMenu = nullptr;
	m_operationToolBar = new iRICToolBar(tr("Operation ToolBar"), iricMainWindow());
	m_selectedItem = nullptr;
	m_dataLoaded = false;

	m_itemModel = new QStandardItemModel(this);
}

GraphicsWindowDataModel::~GraphicsWindowDataModel()
{
	delete m_operationToolBar;
}

void GraphicsWindowDataModel::loadFromCgnsFile(const int fn)
{
	m_rootDataItem->loadFromCgnsFile(fn);
	m_graphicsView->cameraFit();
}

void GraphicsWindowDataModel::saveToCgnsFile(const int fn)
{
	m_rootDataItem->saveToCgnsFile(fn);
}

void GraphicsWindowDataModel::closeCgnsFile()
{
	m_rootDataItem->closeCgnsFile();
}

void GraphicsWindowDataModel::handleObjectBrowserPress(const QModelIndex& index, const QPoint& globalPos)
{
	if (QApplication::mouseButtons() == Qt::RightButton) {
		delete m_rightClickMenu;
		m_rightClickMenu = new QMenu(projectData()->mainWindow());
		QStandardItem* pressedItem = m_itemModel->itemFromIndex(index);
		GraphicsWindowDataItem* dataItem = m_rootDataItem->modelItemFromItem(pressedItem);
		// no corresponding item found.
		if (dataItem == nullptr) {return;}

		dataItem->addCustomMenuItems(m_rightClickMenu);
		if (dataItem->isReorderable()) {
			if (m_rightClickMenu->actions().count() > 0) {
				m_rightClickMenu->addSeparator();
			}
			m_rightClickMenu->addAction(m_objectBrowserView->moveUpAction());
			m_rightClickMenu->addAction(m_objectBrowserView->moveDownAction());
		}
		if (dataItem->isDeletable()) {
			if (m_rightClickMenu->actions().count() > 0) {
				m_rightClickMenu->addSeparator();
			}
			m_rightClickMenu->addAction(m_objectBrowserView->deleteAction());
		}
		QDialog* propDialog = dataItem->propertyDialog(nullptr);
		if (propDialog != nullptr) {
			m_rightClickMenu->addSeparator();
			m_rightClickMenu->addAction(m_objectBrowserView->propertyAction());
			delete propDialog;
		}
		// @todo add custom actions.
		m_rightClickMenu->move(globalPos);
		m_rightClickMenu->show();
	}
}
void GraphicsWindowDataModel::handleObjectBrowserClick(const QModelIndex& index)
{
	// Investigate which node was clicked.
	QStandardItem* clickedItem = m_itemModel->itemFromIndex(index);
	m_rootDataItem->handleItemClick(clickedItem);

	// active dataitem is changed.
	GraphicsWindowDataItem* i = m_rootDataItem->modelItemFromItem(clickedItem);
	if (i != nullptr) {
		m_graphicsView->setActiveDataItem(i);
	}
}
void GraphicsWindowDataModel::handleObjectBrowserDoubleClick(const QModelIndex& index)
{
	// Investigate which node was clicked.
	QStandardItem* clickedItem = m_itemModel->itemFromIndex(index);
	m_rootDataItem->handleItemDoubleClick(clickedItem);
}

void GraphicsWindowDataModel::handleObjectBrowserChange(QStandardItem* changeditem)
{
	m_rootDataItem->handleItemChange(changeditem);
}

void GraphicsWindowDataModel::deleteItem(const QModelIndex& index)
{
	QStandardItem* item = m_itemModel->itemFromIndex(index);
	m_rootDataItem->deleteItem(item);
	m_graphicsView->setActiveDataItem(nullptr);
	// deleting operation is not undo-able!
	iRICUndoStack::instance().clear();
}

void GraphicsWindowDataModel::moveUpItem(const QModelIndex& index)
{
	QStandardItem* item = m_itemModel->itemFromIndex(index);
	if (item->index().row() == 0) {
		// Can not move up!
		m_objectBrowserView->moveUpAction()->setDisabled(true);
		return;
	}
	m_rootDataItem->moveUpItem(item);
}

void GraphicsWindowDataModel::moveDownItem(const QModelIndex& index)
{
	QStandardItem* item = m_itemModel->itemFromIndex(index);
	QStandardItem* pItem = item->parent();
	if (pItem != nullptr && (item->index().row() == pItem->rowCount() - 1)) {
		m_objectBrowserView->moveDownAction()->setDisabled(true);
		return;
	}
	m_rootDataItem->moveDownItem(item);
}

void GraphicsWindowDataModel::showPropertyDialog(const QModelIndex& index)
{
	QStandardItem* item = m_itemModel->itemFromIndex(index);
	GraphicsWindowDataItem* dataItem = m_rootDataItem->modelItemFromItem(item);
	dataItem->showPropertyDialog();
}

void GraphicsWindowDataModel::updateOperationToolBar(const QModelIndex& index, QWidget* /*parent*/)
{
	QStandardItem* item = m_itemModel->itemFromIndex(index);
	GraphicsWindowDataItem* dataItem = m_rootDataItem->modelItemFromItem(item);

	m_operationToolBar->clear();
	m_operationToolBar->addAction(m_objectBrowserView->moveUpAction());
	m_operationToolBar->addAction(m_objectBrowserView->moveDownAction());
	m_operationToolBar->addAction(m_objectBrowserView->deleteAction());

	if (dataItem != nullptr) {
		QAction* sep = m_operationToolBar->addSeparator();
		// add additinal buttons related to currently selected item.
		bool added = dataItem->addToolBarButtons(m_operationToolBar);
		if (! added) {
			m_operationToolBar->removeAction(sep);
		}
		// enable/disable moveup, movedown, delete actions.
		if (dataItem->isReorderable()) {
			dataItem->updateMoveUpDownActions(m_objectBrowserView);
		} else {
			m_objectBrowserView->moveUpAction()->setDisabled(true);
			m_objectBrowserView->moveDownAction()->setDisabled(true);
		}
		m_objectBrowserView->deleteAction()->setEnabled(dataItem->isDeletable());
	} else {
		m_objectBrowserView->moveUpAction()->setDisabled(true);
		m_objectBrowserView->moveDownAction()->setDisabled(true);
		m_objectBrowserView->deleteAction()->setDisabled(true);
	}
}

void GraphicsWindowDataModel::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_objectBrowserView->blockSignals(true);
	m_itemModel->blockSignals(true);
	m_rootDataItem->loadFromProjectMainFile(node);
	m_objectBrowserView->blockSignals(false);
	m_itemModel->blockSignals(false);
	m_graphicsView->mainRenderer()->ResetCameraClippingRange();
	m_graphicsView->render();
}

void GraphicsWindowDataModel::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_rootDataItem->saveToProjectMainFile(writer);
}

void GraphicsWindowDataModel::updateExpandState(ObjectBrowserView* view)
{
	m_rootDataItem->updateExpandState(view);
}

void GraphicsWindowDataModel::reflectExpandState(ObjectBrowserView* view)
{
	view->collapseAll();
	m_rootDataItem->reflectExpandState(view);
}

void GraphicsWindowDataModel::handleObjectBrowserDeselection(const QModelIndex& previous)
{
	// inform the item that it is now deselected.
	QStandardItem* item = m_itemModel->itemFromIndex(previous);
	GraphicsWindowDataItem* dataItem = m_rootDataItem->modelItemFromItem(item);
	if (dataItem) {dataItem->informDeselection(m_graphicsView);}
	m_graphicsView->setActiveDataItem(nullptr);
}

void GraphicsWindowDataModel::handleObjectBrowserSelection(const QModelIndex& current)
{
	// toolbar operation.
	QMainWindow* mainW = mainWindow();
	updateOperationToolBar(current, mainW);
	// inform the item that it is now selected.
	QStandardItem* item = m_itemModel->itemFromIndex(current);
	GraphicsWindowDataItem* dataItem = m_rootDataItem->modelItemFromItem(item);
	m_selectedItem = dataItem;
	if (dataItem) {
		dataItem->informSelection(m_graphicsView);
		m_graphicsView->setActiveDataItem(dataItem);
	}
	// now, update the graphics view.
	graphicsView()->GetRenderWindow()->Render();
}

void GraphicsWindowDataModel::setObjectBrowserView(ObjectBrowserView* v)
{
	m_objectBrowserView = v;
	connect(m_objectBrowserView, SIGNAL(itemSelected(QModelIndex)), this, SLOT(handleObjectBrowserSelection(QModelIndex)));
	connect(m_objectBrowserView, SIGNAL(itemDeselected(QModelIndex)), this, SLOT(handleObjectBrowserDeselection(QModelIndex)));
	connect(m_objectBrowserView, SIGNAL(selectionChanged()), this, SLOT(handleObjectBrowserSelectionChange()));
}

void GraphicsWindowDataModel::fitOnDataLoad()
{
	if (! m_dataLoaded) {
		m_graphicsView->cameraFit();
	}
	m_dataLoaded = true;
}

void GraphicsWindowDataModel::update2Ds()
{
	m_rootDataItem->update2Ds();
}

void GraphicsWindowDataModel::viewOperationEndedGlobal()
{
	m_rootDataItem->viewOperationEndedGlobal(m_graphicsView);
}
