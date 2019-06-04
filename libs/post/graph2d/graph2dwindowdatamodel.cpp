#define _USE_MATH_DEFINES
#include <cmath>

#include "graph2dwindowdatamodel.h"
#include <misc/mathsupport.h>
#include <guibase/irictoolbar.h>
#include <misc/iricundostack.h>
#include "graph2dwindowrootdataitem.h"
#include <guibase/objectbrowserview.h>
#include <guicore/datamodel/vtkgraphicsview.h>
#include <guicore/base/iricmainwindowinterface.h>
#include "graph2dwindowverticalaxissetting.h"
#include "graph2dwindowverticalaxissettingdialog.h"
#include "graph2dwindowview.h"
#include <guicore/project/projectdata.h>

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
#include <QInputDialog>
#include <QLayout>
#include <QLabel>

#include <qwt_plot_canvas.h>
#include <qwt_legend.h>

Graph2dWindowDataModel::Graph2dWindowDataModel(QMainWindow* w, ProjectDataItem* parent)
	: ProjectDataItem(parent)
{
	m_rootDataItem = nullptr;
	m_mainWindow = w;
	m_rightClickMenu = nullptr;
	m_operationToolBar = new iRICToolBar(iricMainWindow());
	m_selectedItem = nullptr;
	m_dataLoaded = false;

	m_itemModel = new QStandardItemModel(this);

	m_leftAxisSetting = new Graph2dWindowVerticalAxisSetting(this);
	m_rightAxisSetting = new Graph2dWindowVerticalAxisSetting(this);
}

Graph2dWindowDataModel::~Graph2dWindowDataModel()
{
	delete m_operationToolBar;
}

void Graph2dWindowDataModel::loadFromCgnsFile(const int fn)
{
	m_rootDataItem->loadFromCgnsFile(fn);
}

void Graph2dWindowDataModel::saveToCgnsFile(const int fn)
{
	m_rootDataItem->saveToCgnsFile(fn);
}

void Graph2dWindowDataModel::closeCgnsFile()
{
	m_rootDataItem->closeCgnsFile();
}

void Graph2dWindowDataModel::handleObjectBrowserPress(const QModelIndex& index, const QPoint& globalPos)
{
	if (QApplication::mouseButtons() == Qt::RightButton) {
		delete m_rightClickMenu;
		m_rightClickMenu = new QMenu(projectData()->mainWindow());
		QStandardItem* pressedItem = m_itemModel->itemFromIndex(index);
		Graph2dWindowDataItem* dataItem = m_rootDataItem->modelItemFromItem(pressedItem);
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

void Graph2dWindowDataModel::handleObjectBrowserDoubleClick(const QModelIndex& index)
{
	// Investigate which node was clicked.
	QStandardItem* clickedItem = m_itemModel->itemFromIndex(index);
	m_rootDataItem->handleItemDoubleClick(clickedItem);
}

void Graph2dWindowDataModel::handleObjectBrowserChange(QStandardItem* changeditem)
{
	m_rootDataItem->handleItemChange(changeditem);
}

void Graph2dWindowDataModel::deleteItem(const QModelIndex& index)
{
	QStandardItem* item = m_itemModel->itemFromIndex(index);
	m_rootDataItem->deleteItem(item);
//	m_graphicsView->setActiveDataItem(nullptr);
	// deleting operation is not undo-able!
	iRICUndoStack::instance().clear();
}

void Graph2dWindowDataModel::moveUpItem(const QModelIndex& index)
{
	QStandardItem* item = m_itemModel->itemFromIndex(index);
	if (item->index().row() == 0) {
		// Can not move up!
		m_objectBrowserView->moveUpAction()->setDisabled(true);
		return;
	}
	m_rootDataItem->moveUpItem(item);
}

void Graph2dWindowDataModel::moveDownItem(const QModelIndex& index)
{
	QStandardItem* item = m_itemModel->itemFromIndex(index);
	QStandardItem* pItem = item->parent();
	if (pItem != nullptr && (item->index().row() == pItem->rowCount() - 1)) {
		m_objectBrowserView->moveDownAction()->setDisabled(true);
		return;
	}
	m_rootDataItem->moveDownItem(item);
}

void Graph2dWindowDataModel::showPropertyDialog(const QModelIndex& index)
{
	QStandardItem* item = m_itemModel->itemFromIndex(index);
	Graph2dWindowDataItem* dataItem = m_rootDataItem->modelItemFromItem(item);
	dataItem->showPropertyDialog();
}

void Graph2dWindowDataModel::updateOperationToolBar(const QModelIndex& index, QWidget* /*parent*/)
{
	QStandardItem* item = m_itemModel->itemFromIndex(index);
	Graph2dWindowDataItem* dataItem = m_rootDataItem->modelItemFromItem(item);

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

void Graph2dWindowDataModel::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_objectBrowserView->blockSignals(true);
	m_itemModel->blockSignals(true);
	m_rootDataItem->loadFromProjectMainFile(node);
	m_objectBrowserView->blockSignals(false);
	m_itemModel->blockSignals(false);
}

void Graph2dWindowDataModel::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_rootDataItem->saveToProjectMainFile(writer);
}

void Graph2dWindowDataModel::updateExpandState(ObjectBrowserView* view)
{
	m_rootDataItem->updateExpandState(view);
}

void Graph2dWindowDataModel::reflectExpandState(ObjectBrowserView* view)
{
	view->collapseAll();
	m_rootDataItem->reflectExpandState(view);
}

void Graph2dWindowDataModel::handleObjectBrowserDeselection(const QModelIndex& /*previous*/)
{
	// inform the item that it is now deselected.
//	QStandardItem* item = m_itemModel->itemFromIndex(previous);
//	Graph2dWindowDataItem* dataItem = m_rootDataItem->modelItemFromItem(item);
//	if (dataItem){dataItem->informDeselection(m_graphicsView);}
//	m_graphicsView->setActiveDataItem(nullptr);
}

void Graph2dWindowDataModel::handleObjectBrowserSelection(const QModelIndex& current)
{
	// toolbar operation.
	QMainWindow* mainW = mainWindow();
	updateOperationToolBar(current, mainW);
	// inform the item that it is now selected.
	QStandardItem* item = m_itemModel->itemFromIndex(current);
	Graph2dWindowDataItem* dataItem = m_rootDataItem->modelItemFromItem(item);
	m_selectedItem = dataItem;
	if (dataItem) {
//		dataItem->informSelection(m_graphicsView);
//		m_graphicsView->setActiveDataItem(dataItem);
	}
	// now, update the graphics view.
//	graphicsView()->GetRenderWindow()->Render();
}

void Graph2dWindowDataModel::setObjectBrowserView(ObjectBrowserView* v)
{
	m_objectBrowserView = v;
	connect(m_objectBrowserView, SIGNAL(itemSelected(QModelIndex)), this, SLOT(handleObjectBrowserSelection(QModelIndex)));
	connect(m_objectBrowserView, SIGNAL(itemDeselected(QModelIndex)), this, SLOT(handleObjectBrowserDeselection(QModelIndex)));
	connect(m_objectBrowserView, SIGNAL(selectionChanged()), this, SLOT(handleObjectBrowserSelectionChange()));
}

void Graph2dWindowDataModel::verticalAxisLeftSetting()
{
	Graph2dWindowVerticalAxisSettingDialog dialog(view());
	double min, max;
	getYAxisValueRange(asLeft, &min, &max);
	dialog.setAutoRange(min, max);
	dialog.setSetting(*m_leftAxisSetting);
	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}
	*m_leftAxisSetting = dialog.setting();
	applyAxisSetting();
	view()->replot();
}

void Graph2dWindowDataModel::verticalAxisRightSetting()
{
	Graph2dWindowVerticalAxisSettingDialog dialog(view());
	double min, max;
	getYAxisValueRange(asRight, &min, &max);
	dialog.setAutoRange(min, max);
	dialog.setSetting(*m_rightAxisSetting);
	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}
	*m_rightAxisSetting = dialog.setting();
	applyAxisSetting();
	view()->replot();
}

void Graph2dWindowDataModel::titleSetting()
{
	bool ok;
	QString newtitle = QInputDialog::getText(view(), tr("Title Setting"), tr("Please input the title."), QLineEdit::Normal, m_title, &ok);
	if (! ok) {return;}
	m_title = newtitle;
	view()->setTitle(newtitle);
	view()->replot();
}

void Graph2dWindowDataModel::setLegendVisible(bool visible)
{
	view()->setLegendVisible(visible);
}
