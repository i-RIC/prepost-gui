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
//	graphicsView() = dynamic_cast<VTKGraphicsView*>(w->centralWidget());
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

QToolBar* GraphicsWindowDataModel::operationToolBar() const
{
	return m_operationToolBar;
}

QStandardItemModel* GraphicsWindowDataModel::itemModel() const
{
	return m_itemModel;
}

void GraphicsWindowDataModel::loadFromCgnsFile(const int fn)
{
	m_rootDataItem->loadFromCgnsFile(fn);
	graphicsView()->cameraFit();
}

void GraphicsWindowDataModel::saveToCgnsFile(const int fn)
{
	m_rootDataItem->saveToCgnsFile(fn);
}

void GraphicsWindowDataModel::closeCgnsFile()
{
	m_rootDataItem->closeCgnsFile();
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

ObjectBrowserView* GraphicsWindowDataModel::objectBrowserView() const
{
	return m_objectBrowserView;
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
		graphicsView()->cameraFit();
	}
	m_dataLoaded = true;
}

void GraphicsWindowDataModel::viewOperationEndedGlobal()
{
	m_rootDataItem->viewOperationEndedGlobal(graphicsView());
}

void GraphicsWindowDataModel::handleObjectBrowserPress(const QModelIndex& index, const QPoint& globalPos)
{
	QStandardItem* pressedItem = m_itemModel->itemFromIndex(index);
	GraphicsWindowDataItem* dataItem = m_rootDataItem->modelItemFromItem(pressedItem);
	if (dataItem == nullptr) {return;}

	if (QApplication::mouseButtons() != Qt::RightButton) {return;}

	delete m_rightClickMenu;
	m_rightClickMenu = new QMenu(projectData()->mainWindow());

	dataItem->addCustomMenuItems(m_rightClickMenu);

	// add moveUp, moveDown actions if needed
	if (dataItem->isReorderable()) {
		if (m_rightClickMenu->actions().count() > 0) {
			m_rightClickMenu->addSeparator();
		}
		m_rightClickMenu->addAction(m_objectBrowserView->moveUpAction());
		m_rightClickMenu->addAction(m_objectBrowserView->moveDownAction());
	}

	// add delete action if needed
	if (dataItem->isDeletable()) {
		if (m_rightClickMenu->actions().count() > 0) {
			m_rightClickMenu->addSeparator();
		}
		m_rightClickMenu->addAction(m_objectBrowserView->deleteAction());
	}

	// add property action if needed
	QDialog* propDialog = dataItem->propertyDialog(nullptr);
	if (propDialog != nullptr) {
		m_rightClickMenu->addSeparator();
		m_rightClickMenu->addAction(m_objectBrowserView->propertyAction());
		delete propDialog;
	}

	// show right-clicking menu
	m_rightClickMenu->move(globalPos);
	m_rightClickMenu->show();
}

void GraphicsWindowDataModel::handleObjectBrowserClick(const QModelIndex& index)
{
	// Investigate which node was clicked.
	QStandardItem* clickedItem = m_itemModel->itemFromIndex(index);
	m_rootDataItem->handleItemClick(clickedItem);

	GraphicsWindowDataItem* i = m_rootDataItem->modelItemFromItem(clickedItem);
	if (i == nullptr) {return;}

	graphicsView()->setActiveDataItem(i);
}

void GraphicsWindowDataModel::handleObjectBrowserDoubleClick(const QModelIndex& index)
{
	QStandardItem* clickedItem = m_itemModel->itemFromIndex(index);
	m_rootDataItem->handleItemDoubleClick(clickedItem);
}

void GraphicsWindowDataModel::handleObjectBrowserChange(QStandardItem* changeditem)
{
	m_rootDataItem->handleItemChange(changeditem);
}

void GraphicsWindowDataModel::deleteItem(const QModelIndex& index, bool noDraw)
{
	QStandardItem* item = m_itemModel->itemFromIndex(index);
	m_rootDataItem->deleteItem(item, noDraw);
	graphicsView()->setActiveDataItem(nullptr);
	// deleting operation is not undo-able!
	iRICUndoStack::instance().clear();
}

void GraphicsWindowDataModel::undoableDeleteItem(const QModelIndex& index, bool noDraw)
{
	QStandardItem* item = m_itemModel->itemFromIndex(index);
	GraphicsWindowDataItem* dataItem = m_rootDataItem->modelItemFromItem(item);
	GraphicsWindowDataItem* parentDataItem = m_rootDataItem->modelItemFromItem(item->parent());
	parentDataItem->undoableDeleteItem(dataItem, noDraw);
	graphicsView()->setActiveDataItem(nullptr);
}

void GraphicsWindowDataModel::moveUpItem(const QModelIndex& index)
{
	QStandardItem* item = m_itemModel->itemFromIndex(index);
	if (item->index().row() == 0) {return;}

	m_rootDataItem->moveUpItem(item);
}

void GraphicsWindowDataModel::moveDownItem(const QModelIndex& index)
{
	QStandardItem* item = m_itemModel->itemFromIndex(index);
	QStandardItem* pItem = item->parent();
	if (pItem != nullptr && (item->index().row() == pItem->rowCount() - 1)) {return;}

	m_rootDataItem->moveDownItem(item);
}

void GraphicsWindowDataModel::showAddDialog(const QModelIndex& index)
{
	QStandardItem* item = m_itemModel->itemFromIndex(index);
	GraphicsWindowDataItem* dataItem = m_rootDataItem->modelItemFromItem(item);
	dataItem->showAddDialog();
}

void GraphicsWindowDataModel::showPropertyDialog(const QModelIndex& index)
{
	QStandardItem* item = m_itemModel->itemFromIndex(index);
	GraphicsWindowDataItem* dataItem = m_rootDataItem->modelItemFromItem(item);
	dataItem->showPropertyDialog();
}

void GraphicsWindowDataModel::updateOperationToolBar(const QModelIndex& index, QWidget*)
{
	QStandardItem* item = m_itemModel->itemFromIndex(index);
	GraphicsWindowDataItem* dataItem = m_rootDataItem->modelItemFromItem(item);

	m_operationToolBar->clear();
	m_operationToolBar->addAction(m_objectBrowserView->moveUpAction());
	m_operationToolBar->addAction(m_objectBrowserView->moveDownAction());
	m_operationToolBar->addAction(m_objectBrowserView->deleteAction());

	if (dataItem == nullptr) {
		m_objectBrowserView->moveUpAction()->setDisabled(true);
		m_objectBrowserView->moveDownAction()->setDisabled(true);
		m_objectBrowserView->deleteAction()->setDisabled(true);

		return;
	}

	QAction* sep = m_operationToolBar->addSeparator();
	// add additinal buttons related to currently selected item.
	bool added = dataItem->addToolBarButtons(m_operationToolBar);
	if (! added) {
		m_operationToolBar->removeAction(sep);
	}

	// enable or disable moveup, movedown, delete actions.
	if (dataItem->isReorderable()) {
		dataItem->updateMoveUpDownActions(m_objectBrowserView);
	} else {
		m_objectBrowserView->moveUpAction()->setDisabled(true);
		m_objectBrowserView->moveDownAction()->setDisabled(true);
	}

	m_objectBrowserView->deleteAction()->setEnabled(dataItem->isDeletable());
}

void GraphicsWindowDataModel::handleObjectBrowserSelectionChange()
{}

void GraphicsWindowDataModel::handleObjectBrowserSelection(const QModelIndex& current)
{
	QMainWindow* mainW = mainWindow();
	updateOperationToolBar(current, mainW);

	QStandardItem* item = m_itemModel->itemFromIndex(current);
	GraphicsWindowDataItem* dataItem = m_rootDataItem->modelItemFromItem(item);
	m_selectedItem = dataItem;

	if (dataItem) {
		dataItem->informSelection(graphicsView());
		graphicsView()->setActiveDataItem(dataItem);
	}

	graphicsView()->GetRenderWindow()->Render();
}

void GraphicsWindowDataModel::handleObjectBrowserDeselection(const QModelIndex& previous)
{
	QStandardItem* item = m_itemModel->itemFromIndex(previous);
	GraphicsWindowDataItem* dataItem = m_rootDataItem->modelItemFromItem(item);

	if (dataItem) {dataItem->informDeselection(graphicsView());}

	graphicsView()->setActiveDataItem(nullptr);
}

void GraphicsWindowDataModel::update2Ds()
{
	m_rootDataItem->update2Ds();
}

void GraphicsWindowDataModel::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_objectBrowserView->blockSignals(true);

	m_itemModel->blockSignals(true);
	m_rootDataItem->loadFromProjectMainFile(node);

	m_objectBrowserView->blockSignals(false);

	m_itemModel->blockSignals(false);
	graphicsView()->mainRenderer()->ResetCameraClippingRange();
	graphicsView()->render();
}

void GraphicsWindowDataModel::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_rootDataItem->saveToProjectMainFile(writer);
}
