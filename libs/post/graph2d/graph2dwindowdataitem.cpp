#include "graph2dwindowdataitem.h"
#include "graph2dwindowrootdataitem.h"
#include "graph2dwindowdatamodel.h"
#include <misc/iricundostack.h>
#include "graph2dwindowview.h"

#include <QStandardItem>
#include <QMessageBox>
#include <QDomNode>
#include <QAction>
#include <QXmlStreamWriter>
#include <QTreeView>
#include <QMainWindow>
#include <QUndoCommand>

Graph2dWindowDataItem::Graph2dWindowDataItem(const QString& itemlabel, Graph2dWindowDataItem* parent)
	: ProjectDataItem(parent)
{
	m_standardItem = new QStandardItem(itemlabel);
	if (dynamic_cast<Graph2dWindowRootDataItem*>(parent) == 0){
		parent->standardItem()->appendRow(m_standardItem);
	}
	init();
}
Graph2dWindowDataItem::Graph2dWindowDataItem(const QString& itemlabel, const QIcon& icon, Graph2dWindowDataItem* parent)
	: ProjectDataItem(parent)
{
	m_standardItem = new QStandardItem(icon, itemlabel);
	if (dynamic_cast<Graph2dWindowRootDataItem*>(parent) == 0){
		parent->standardItem()->appendRow(m_standardItem);
	}
	init();
}

Graph2dWindowDataItem::Graph2dWindowDataItem(ProjectDataItem* parent)
	: ProjectDataItem(parent)
{
	m_standardItem = 0;
	init();
}

Graph2dWindowDataItem::~Graph2dWindowDataItem(){
	// delete all child items.
	m_isDestructing = true;
	QList <Graph2dWindowDataItem*>::iterator it;
	for (it = m_childItems.begin(); it != m_childItems.end(); ++it){
		delete *it;
	}
	ProjectDataItem* tmp_parent = parent();
	if (tmp_parent != 0){
		Graph2dWindowDataItem* p = dynamic_cast<Graph2dWindowDataItem*>(tmp_parent);
		if (p != 0){p->unregisterChild(this);}
	}
	m_isDestructing = false;

	// remove the item from QStandardItemModel.
	QStandardItem* item = m_standardItem;
	if (item != 0){
		if (item->parent() == 0 || item->parent()->row() == -1){
			// maybe this is the top level item of the model
			QStandardItemModel* model = dataModel()->itemModel();
			QStandardItem* i = model->item(item->row());
			if (i == item){
				// yes, it is!
				dataModel()->itemModel()->removeRow(item->row());
			}
		}else{
			if (item->parent() != 0){
				QStandardItem* i = item->parent()->child(item->row());
				if (i == item){
					item->parent()->removeRow(item->row());
				}
			}
		}
	}
}

bool Graph2dWindowDataItem::isEnabled()
{
	if (m_standardItem == 0){return false;}
	return (m_standardItem->checkState() == Qt::Checked);
}

void Graph2dWindowDataItem::setEnabled(bool enabled)
{
	if (m_standardItem == 0){return;}
	if (enabled){
		m_standardItem->setCheckState(Qt::Checked);
	}else{
		m_standardItem->setCheckState(Qt::Unchecked);
	}
}

void Graph2dWindowDataItem::unregisterChild(Graph2dWindowDataItem* child)
{
	QList <Graph2dWindowDataItem*>::iterator it;
	for (it = m_childItems.begin(); it != m_childItems.end(); ++it){
		if (*it == child){
			m_childItems.erase(it);
			return;
		}
	}
}

void Graph2dWindowDataItem::init()
{
	if (m_standardItem){
		m_standardItem->setEditable(false);
	}
	m_standardItemCopy = 0;
	m_isDeletable = true;
	m_isReorderable = false;
	m_isDestructing = false;

	m_isPushing = false;
	m_isCommandExecuting = false;
}

void Graph2dWindowDataItem::innerUpdateItemMap(QMap<QStandardItem*, Graph2dWindowDataItem*>& map)
{
	map.insert(m_standardItem, this);
	QList<Graph2dWindowDataItem*>::iterator it;
	for (it = m_childItems.begin(); it != m_childItems.end(); ++it){
		(*it)->innerUpdateItemMap(map);
	}
}

/// The class to store standard item change information.
/**
	* This class stores only displayRole data and checkState data.
	*/
class Graph2dWindowDataItemStandardItemChangeCommand : public QUndoCommand
{
public:
	Graph2dWindowDataItemStandardItemChangeCommand(Graph2dWindowDataItem* item)
		: QUndoCommand(QObject::tr("Object Browser Item Change"))
	{
		m_oldDisplayText = item->m_standardItemCopy->data(Qt::DisplayRole);
		m_oldCheckState = item->m_standardItemCopy->data(Qt::CheckStateRole);

		m_newDisplayText = item->m_standardItem->data(Qt::DisplayRole);
		m_newCheckState = item->m_standardItem->data(Qt::CheckStateRole);

		m_item = item;
	}
	void redo()
	{
		m_item->setIsCommandExecuting(true);
//		m_item->m_standardItemCopy->setData(m_item->m_standardItem->data(Qt::DisplayRole), Qt::DisplayRole);
//		m_item->m_standardItemCopy->setData(m_item->m_standardItem->data(Qt::CheckStateRole), Qt::CheckStateRole);

		m_item->m_standardItemCopy->setData(m_newDisplayText, Qt::DisplayRole);
		m_item->m_standardItemCopy->setData(m_newCheckState, Qt::CheckStateRole);

		m_item->m_standardItem->setData(m_newDisplayText, Qt::DisplayRole);
		m_item->m_standardItem->setData(m_newCheckState, Qt::CheckStateRole);

		m_item->setModified();
		m_item->updateVisibility();
		m_item->setIsCommandExecuting(false);
	}
	void undo()
	{
		m_item->setIsCommandExecuting(true);
//		m_item->m_standardItemCopy->setData(m_item->m_standardItem->data(Qt::DisplayRole), Qt::DisplayRole);
//		m_item->m_standardItemCopy->setData(m_item->m_standardItem->data(Qt::CheckStateRole), Qt::CheckStateRole);

		m_item->m_standardItemCopy->setData(m_oldDisplayText, Qt::DisplayRole);
		m_item->m_standardItemCopy->setData(m_oldCheckState, Qt::CheckStateRole);

		m_item->m_standardItem->setData(m_oldDisplayText, Qt::DisplayRole);
		m_item->m_standardItem->setData(m_oldCheckState, Qt::CheckStateRole);

		m_item->setModified();
		m_item->updateVisibility();
		m_item->setIsCommandExecuting(false);
	}
private:
	QVariant m_oldDisplayText;
	QVariant m_oldCheckState;

	QVariant m_newDisplayText;
	QVariant m_newCheckState;
	Graph2dWindowDataItem* m_item;
};


void Graph2dWindowDataItem::handleStandardItemChange()
{
	if (m_isCommandExecuting){return;}
	iRICUndoStack::instance().push(new Graph2dWindowDataItemStandardItemChangeCommand(this));
}

void Graph2dWindowDataItem::loadFromCgnsFile(const int fn)
{
	QList<Graph2dWindowDataItem*>::iterator it;
	for (it = m_childItems.begin(); it != m_childItems.end(); ++it){
		(*it)->loadFromCgnsFile(fn);
	}
}

void Graph2dWindowDataItem::saveToCgnsFile(const int fn)
{
	QList<Graph2dWindowDataItem*>::iterator it;
	for (it = m_childItems.begin(); it != m_childItems.end(); ++it){
		(*it)->saveToCgnsFile(fn);
	}
}

void Graph2dWindowDataItem::closeCgnsFile()
{
	QList<Graph2dWindowDataItem*>::iterator it;
	for (it = m_childItems.begin(); it != m_childItems.end(); ++it){
		(*it)->closeCgnsFile();
	}
}

void Graph2dWindowDataItem::loadCheckState(const QDomNode& node)
{
	if (m_standardItem == 0){return;}
	if (m_standardItem->isCheckable()){
		m_standardItem->setCheckState(static_cast<Qt::CheckState>(node.toElement().attribute("checkState", "0").toInt()));
	}
}

void Graph2dWindowDataItem::saveCheckState(QXmlStreamWriter& writer)
{
	if (m_standardItem == 0){return;}
	if (m_standardItem->isCheckable()){
		QString checkState;
		checkState.setNum(m_standardItem->checkState());
		writer.writeAttribute("checkState", checkState);
	}
}

void Graph2dWindowDataItem::loadExpandState(const QDomNode& node)
{
	m_isExpanded = (node.toElement().attribute("isExpanded", "false") == "true");
}

void Graph2dWindowDataItem::saveExpandState(QXmlStreamWriter& writer)
{
	QString strExpanded;
	if (m_isExpanded){
		strExpanded = "true";
	}else{
		strExpanded = "false";
	}
	writer.writeAttribute("isExpanded", strExpanded);
}
void Graph2dWindowDataItem::updateExpandState(QTreeView* view)
{
	if (m_standardItem != 0){
		m_isExpanded = view->isExpanded(m_standardItem->index());
	}
	QList<Graph2dWindowDataItem*>::iterator it;
	for (it = m_childItems.begin(); it != m_childItems.end(); ++it){
		(*it)->updateExpandState(view);
	}
}

void Graph2dWindowDataItem::reflectExpandState(QTreeView* view)
{
	if (m_standardItem != 0){
		view->setExpanded(m_standardItem->index(), m_isExpanded);
	}
	QList<Graph2dWindowDataItem*>::iterator it;
	for (it = m_childItems.begin(); it != m_childItems.end(); ++it){
		(*it)->reflectExpandState(view);
	}
}

void Graph2dWindowDataItem::loadFromProjectMainFile(const QDomNode& node)
{
	loadCheckState(node);
	loadExpandState(node);
	ProjectDataItem::loadFromProjectMainFile(node);
}

void Graph2dWindowDataItem::saveToProjectMainFile(QXmlStreamWriter& writer)
{
	saveCheckState(writer);
	saveExpandState(writer);
	ProjectDataItem::saveToProjectMainFile(writer);
}

void Graph2dWindowDataItem::updateVisibility()
{
	bool ancientVisible = isAncientChecked();
	updateVisibility(ancientVisible);

	renderView();
}


void Graph2dWindowDataItem::updateVisibility(bool visible)
{
	bool my_visible = true;
	if (m_standardItem == 0){
		my_visible = true;
	}else if (m_standardItem->isCheckable()){
		switch (m_standardItem->checkState()){
		case Qt::Checked:
		case Qt::PartiallyChecked:
			my_visible = true;
			break;
		case Qt::Unchecked:
			my_visible = false;
			break;
		}
	}
	visible = visible && my_visible;

	setVisible(visible);
	// cascade to update the visibility of actors those are
	// handled by the child instances.
	QList<Graph2dWindowDataItem*>::iterator c_it;
	for (c_it = m_childItems.begin(); c_it != m_childItems.end(); ++c_it){
		(*c_it)->updateVisibility(visible);
	}
}

void Graph2dWindowDataItem::renderView()
{
	dataModel()->view()->replot();
}

QMainWindow* Graph2dWindowDataItem::mainWindow()
{
	return dataModel()->mainWindow();
}

bool Graph2dWindowDataItem::isAncientChecked()
{
	QStandardItem* i = dynamic_cast<Graph2dWindowDataItem*>(parent())->m_standardItem;
	if (i == 0){return true;}
	if (i->isCheckable() && i->checkState() == Qt::Unchecked){
		return false;
	}
	return dynamic_cast<Graph2dWindowDataItem*>(parent())->isAncientChecked();
}

void Graph2dWindowDataItem::moveUp()
{
	// reorder the standard item.
	QStandardItem* parentItem = dynamic_cast<Graph2dWindowDataItem*>(parent())->standardItem();
	int currentRow = m_standardItem->row();
	QList<QStandardItem*> items = parentItem->takeRow(currentRow);
	parentItem->insertRows(currentRow - 1, items);

	// reorder the m_childList of parent.
	Graph2dWindowDataItem* tmpparent = dynamic_cast<Graph2dWindowDataItem*>(parent());
	QList<Graph2dWindowDataItem*>::iterator it, it2;
	for (it = tmpparent->m_childItems.begin(); it != tmpparent->m_childItems.end(); ++it){
		if ((*it) == this){
			it2 = it;
			-- it2;
			tmpparent->m_childItems.erase(it);
			tmpparent->m_childItems.insert(it2, this);
			break;
		}
	}
	// update the ZDepthRange.
	tmpparent->updateZDepthRange();
	renderView();
}

void Graph2dWindowDataItem::moveDown()
{
	// reorder the standard item.
	QStandardItem* parentItem = dynamic_cast<Graph2dWindowDataItem*>(parent())->standardItem();
	int currentRow = m_standardItem->row();
	QList<QStandardItem*> items = parentItem->takeRow(currentRow);
	parentItem->insertRows(currentRow + 1, items);

	// reorder the m_childList of parent.
	Graph2dWindowDataItem* tmpparent = dynamic_cast<Graph2dWindowDataItem*>(parent());
	QList<Graph2dWindowDataItem*>::iterator it, it2;
	for (it = tmpparent->m_childItems.begin(); it != tmpparent->m_childItems.end(); ++it){
		if ((*it) == this){
			it2 = it;
			++ it2;
			++ it2;
			tmpparent->m_childItems.erase(it);
			tmpparent->m_childItems.insert(it2, this);
			break;
		}
	}
	// update the ZDepthRange.
	tmpparent->updateZDepthRange();
	renderView();
}

void Graph2dWindowDataItem::showPropertyDialog()
{
	QDialog* propDialog = propertyDialog(mainWindow());
	if (propDialog == 0){return;}
	int result = propDialog->exec();
	if (result == QDialog::Accepted){
		handlePropertyDialogAccepted(propDialog);
	}
	delete propDialog;
}

void Graph2dWindowDataItem::setZDepthRange(const ZDepthRange& newrange)
{
	m_zDepthRange = newrange;
	assignActionZValues(newrange);
}

void Graph2dWindowDataItem::updateZDepthRange()
{
	updateZDepthRangeItemCount();
	setZDepthRange(m_zDepthRange);
}

void Graph2dWindowDataItem::assignActionZValues(const ZDepthRange& range)
{
	if (m_childItems.count() == 0){return;}

	/// the default behavior is to set ZDepthRanges to child items.
	double rangeWidth = range.width();
	double divNum = 0;
	divNum += m_childItems.count() - 1;
	QList<Graph2dWindowDataItem*>::iterator it;
	for (it = m_childItems.begin(); it != m_childItems.end(); ++it){
		int itemCount = ((*it)->zDepthRange().itemCount() - 1);
		if (itemCount > 0){
			divNum += itemCount;
		}
	}
	if (divNum == 0){divNum = 1;}
	double divWidth = rangeWidth / divNum;
	double max = range.max();
	for (it = m_childItems.begin(); it != m_childItems.end(); ++it){
		int itemCount = ((*it)->zDepthRange().itemCount() - 1);
		int itemCount2 = 0;
		if (itemCount > 0){
			itemCount2 = itemCount;
		}
		double min = max - itemCount2 * divWidth;
		if (min < range.min()){min = range.min();}
		ZDepthRange r = (*it)->zDepthRange();
		r.setMin(min);
		r.setMax(max);
		(*it)->setZDepthRange(r);
		max = min - divWidth;
	}
}

QStringList Graph2dWindowDataItem::containedFiles()
{
	QStringList ret;
	QList<Graph2dWindowDataItem*>::iterator it;
	for (it = m_childItems.begin(); it != m_childItems.end(); ++it){
		ret << (*it)->containedFiles();
	}
	return ret;
}

void Graph2dWindowDataItem::updateZDepthRangeItemCount()
{
	// update the ZDepthRange itemcount of child items first.
	QList <Graph2dWindowDataItem*>::iterator it;
	int sum = 0;
	for (it = m_childItems.begin(); it != m_childItems.end(); ++it){
		(*it)->updateZDepthRangeItemCount();
		sum += (*it)->zDepthRange().itemCount();
	}
	m_zDepthRange.setItemCount(sum);
}
