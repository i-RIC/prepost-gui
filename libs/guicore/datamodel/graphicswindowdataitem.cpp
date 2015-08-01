#include "../project/projectdata.h"
#include "../project/projectmainfile.h"
#include "graphicswindowdataitem.h"
#include "graphicswindowdatamodel.h"
#include "graphicswindowrootdataitem.h"
#include "vtkgraphicsview.h"

#include <misc/iricundostack.h>
#include <memory>

#include <QAction>
#include <QDomNode>
#include <QMainWindow>
#include <QMessageBox>
#include <QStandardItem>
#include <QTreeView>
#include <QUndoCommand>
#include <QXmlStreamWriter>

#include <vtkActor.h>
#include <vtkActor2DCollection.h>
#include <vtkActorCollection.h>
#include <vtkCollectionIterator.h>
#include <vtkRenderWindow.h>

GraphicsWindowDataItem::GraphicsWindowDataItem(const QString& itemlabel, GraphicsWindowDataItem* parent)
	: ProjectDataItem(parent)
{
	m_standardItem = new QStandardItem(itemlabel);
	if (dynamic_cast<GraphicsWindowRootDataItem*>(parent) == nullptr) {
		parent->standardItem()->appendRow(m_standardItem);
	}
	init();
}
GraphicsWindowDataItem::GraphicsWindowDataItem(const QString& itemlabel, const QIcon& icon, GraphicsWindowDataItem* parent)
	: ProjectDataItem(parent)
{
	m_standardItem = new QStandardItem(icon, itemlabel);
	if (dynamic_cast<GraphicsWindowRootDataItem*>(parent) == nullptr) {
		parent->standardItem()->appendRow(m_standardItem);
	}
	init();
}

GraphicsWindowDataItem::GraphicsWindowDataItem(ProjectDataItem* parent)
	: ProjectDataItem(parent)
{
	m_standardItem = nullptr;
	init();
}

GraphicsWindowDataItem::~GraphicsWindowDataItem()
{
	// delete all child items.
	m_isDestructing = true;
	clearChildItems();

	ProjectDataItem* tmp_parent = parent();
	if (tmp_parent != nullptr) {
		GraphicsWindowDataItem* p = dynamic_cast<GraphicsWindowDataItem*>(tmp_parent);
		if (p != nullptr) {p->unregisterChild(this);}
	}
	m_isDestructing = false;

	// remove the item from QStandardItemModel.
	QStandardItem* item = m_standardItem;
	if (item != nullptr) {
		if (item->parent() == nullptr || item->parent()->row() == -1) {
			// maybe this is the top level item of the model
			QStandardItemModel* model = dataModel()->itemModel();
			QStandardItem* i = model->item(item->row());
			if (i == item) {
				// yes, it is!
				dataModel()->itemModel()->removeRow(item->row());
			}
		} else {
			if (item->parent() != nullptr) {
				QStandardItem* i = item->parent()->child(item->row());
				if (i == item) {
					item->parent()->removeRow(item->row());
				}
			}
		}
	}
	m_actorCollection->Delete();
	m_actor2DCollection->Delete();
}

bool GraphicsWindowDataItem::isEnabled() const
{
	if (m_standardItem == nullptr) {return false;}
	return (m_standardItem->checkState() == Qt::Checked);
}

void GraphicsWindowDataItem::setEnabled(bool enabled)
{
	if (m_standardItem == nullptr) {return;}
	if (enabled) {
		m_standardItem->setCheckState(Qt::Checked);
	} else {
		m_standardItem->setCheckState(Qt::Unchecked);
	}
}

void GraphicsWindowDataItem::unregisterChild(GraphicsWindowDataItem* child)
{
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		if (*it == child) {
			m_childItems.erase(it);
			return;
		}
	}
}

void GraphicsWindowDataItem::init()
{
	if (m_standardItem) {
		m_standardItem->setEditable(false);
	}
	m_standardItemCopy = nullptr;
	m_isDeletable = true;
	m_isReorderable = false;
	m_isDestructing = false;

	m_isCommandExecuting = false;

	m_actorCollection = vtkActorCollection::New();
	m_actor2DCollection = vtkActor2DCollection::New();
}

void GraphicsWindowDataItem::innerUpdateItemMap(QMap<QStandardItem*, GraphicsWindowDataItem*>& map)
{
	if (m_standardItem != nullptr) {
		map.insert(m_standardItem, this);
	}
	for (GraphicsWindowDataItem* child : m_childItems) {
		child->innerUpdateItemMap(map);
	}
}

void GraphicsWindowDataItem::handleStandardItemChange()
{
	if (m_isCommandExecuting) {return;}
	iRICUndoStack::instance().push(new GraphicsWindowDataItemStandardItemChangeCommand(this));
}

void GraphicsWindowDataItem::loadFromCgnsFile(const int fn)
{
	for (GraphicsWindowDataItem* child : m_childItems) {
		child->loadFromCgnsFile(fn);
	}
}

void GraphicsWindowDataItem::saveToCgnsFile(const int fn)
{
	for (GraphicsWindowDataItem* child : m_childItems) {
		child->saveToCgnsFile(fn);
	}
}

void GraphicsWindowDataItem::closeCgnsFile()
{
	for (GraphicsWindowDataItem* child : m_childItems) {
		child->closeCgnsFile();
	}
}

void GraphicsWindowDataItem::loadCheckState(const QDomNode& node)
{
	if (m_standardItem == nullptr) {return;}
	if (m_standardItem->isCheckable()) {
		m_standardItem->setCheckState(static_cast<Qt::CheckState>(node.toElement().attribute("checkState", "0").toInt()));
	}
}

void GraphicsWindowDataItem::saveCheckState(QXmlStreamWriter& writer)
{
	if (m_standardItem == nullptr) {return;}
	if (m_standardItem->isCheckable()) {
		QString checkState;
		checkState.setNum(m_standardItem->checkState());
		writer.writeAttribute("checkState", checkState);
	}
}

void GraphicsWindowDataItem::loadExpandState(const QDomNode& node)
{
	m_isExpanded = (node.toElement().attribute("isExpanded", "false") == "true");
}

void GraphicsWindowDataItem::saveExpandState(QXmlStreamWriter& writer)
{
	QString strExpanded;
	if (m_isExpanded) {
		strExpanded = "true";
	} else {
		strExpanded = "false";
	}
	writer.writeAttribute("isExpanded", strExpanded);
}
void GraphicsWindowDataItem::updateExpandState(QTreeView* view)
{
	if (m_standardItem != nullptr) {
		m_isExpanded = view->isExpanded(m_standardItem->index());
	}
	for (GraphicsWindowDataItem* child : m_childItems) {
		child->updateExpandState(view);
	}
}

void GraphicsWindowDataItem::reflectExpandState(QTreeView* view)
{
	if (m_standardItem != nullptr) {
		view->setExpanded(m_standardItem->index(), m_isExpanded);
	}
	for (GraphicsWindowDataItem* child : m_childItems) {
		child->reflectExpandState(view);
	}
}

void GraphicsWindowDataItem::loadFromProjectMainFile(const QDomNode& node)
{
	loadCheckState(node);
	loadExpandState(node);
	ProjectDataItem::loadFromProjectMainFile(node);
}

void GraphicsWindowDataItem::saveToProjectMainFile(QXmlStreamWriter& writer)
{
	saveCheckState(writer);
	saveExpandState(writer);
	ProjectDataItem::saveToProjectMainFile(writer);
}

void GraphicsWindowDataItem::updateVisibility()
{
	bool ancientVisible = isAncientChecked();
	updateVisibility(ancientVisible);

	renderGraphicsView();
}


void GraphicsWindowDataItem::updateVisibilityWithoutRendering()
{
	bool ancientVisible = isAncientChecked();
	updateVisibility(ancientVisible);
}

void GraphicsWindowDataItem::updateVisibility(bool visible)
{
	bool my_visible = true;
	if (m_standardItem == nullptr) {
		my_visible = true;
	} else if (m_standardItem->isCheckable()) {
		switch (m_standardItem->checkState()) {
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
	vtkCollectionIterator* it = m_actorCollection->NewIterator();
	it->GoToFirstItem();
	// update the visibility of actors those are handled
	// by this instance.
	while (!it->IsDoneWithTraversal()) {
		vtkActor* actor = vtkActor::SafeDownCast(it->GetCurrentObject());
		actor->SetVisibility(visible);
		it->GoToNextItem();
	}
	it->Delete();

	it = m_actor2DCollection->NewIterator();
	it->GoToFirstItem();
	// update the visibility of actors those are handled
	// by this instance.
	while (!it->IsDoneWithTraversal()) {
		vtkActor2D* actor = vtkActor2D::SafeDownCast(it->GetCurrentObject());
		actor->SetVisibility(visible);
		it->GoToNextItem();
	}
	it->Delete();

	// cascade to update the visibility of actors those are
	// handled by the child instances.
	for (GraphicsWindowDataItem* child : m_childItems) {
		child->updateVisibility(visible);
	}
}

vtkRenderer* GraphicsWindowDataItem::renderer() const
{
	return dataModel()->graphicsView()->mainRenderer();
}

void GraphicsWindowDataItem::renderGraphicsView()
{
	dataModel()->graphicsView()->GetRenderWindow()->Render();
}

QMainWindow* GraphicsWindowDataItem::mainWindow() const
{
	return dataModel()->mainWindow();
}

bool GraphicsWindowDataItem::isAncientChecked() const
{
	QStandardItem* i = dynamic_cast<GraphicsWindowDataItem*>(parent())->m_standardItem;
	if (i == nullptr) {return true;}
	if (i->isCheckable() && i->checkState() == Qt::Unchecked) {
		return false;
	}
	return dynamic_cast<GraphicsWindowDataItem*>(parent())->isAncientChecked();
}

void GraphicsWindowDataItem::moveUp()
{
	// reorder the standard item.
	QStandardItem* parentItem = dynamic_cast<GraphicsWindowDataItem*>(parent())->standardItem();
	int currentRow = m_standardItem->row();
	QList<QStandardItem*> items = parentItem->takeRow(currentRow);
	parentItem->insertRows(currentRow - 1, items);

	// reorder the m_childList of parent.
	GraphicsWindowDataItem* tmpparent = dynamic_cast<GraphicsWindowDataItem*>(parent());
	for (auto it = tmpparent->m_childItems.begin(); it != tmpparent->m_childItems.end(); ++it) {
		if ((*it) == this) {
			auto it2 = it;
			-- it2;
			tmpparent->m_childItems.erase(it);
			tmpparent->m_childItems.insert(it2, this);
			break;
		}
	}
	// update the ZDepthRange.
	tmpparent->updateZDepthRange();
	renderGraphicsView();
}

void GraphicsWindowDataItem::moveDown()
{
	// reorder the standard item.
	QStandardItem* parentItem = dynamic_cast<GraphicsWindowDataItem*>(parent())->standardItem();
	int currentRow = m_standardItem->row();
	QList<QStandardItem*> items = parentItem->takeRow(currentRow);
	parentItem->insertRows(currentRow + 1, items);

	// reorder the m_childList of parent.
	GraphicsWindowDataItem* tmpparent = dynamic_cast<GraphicsWindowDataItem*>(parent());
	for (auto it = tmpparent->m_childItems.begin(); it != tmpparent->m_childItems.end(); ++it) {
		if ((*it) == this) {
			auto it2 = it;
			++ it2;
			++ it2;
			tmpparent->m_childItems.erase(it);
			tmpparent->m_childItems.insert(it2, this);
			break;
		}
	}
	// update the ZDepthRange.
	tmpparent->updateZDepthRange();
	renderGraphicsView();
}

void GraphicsWindowDataItem::showPropertyDialog()
{
	QDialog* propDialog = propertyDialog(mainWindow());
	if (propDialog == nullptr) {return;}
	int result = propDialog->exec();
	if (result == QDialog::Accepted) {
		handlePropertyDialogAccepted(propDialog);
	}
	delete propDialog;
}

void GraphicsWindowDataItem::setZDepthRange(const ZDepthRange& newrange)
{
	m_zDepthRange = newrange;
	assignActorZValues(newrange);
}

void GraphicsWindowDataItem::updateZDepthRange()
{
	updateZDepthRangeItemCount();
	setZDepthRange(m_zDepthRange);
}

void GraphicsWindowDataItem::assignActorZValues(const ZDepthRange& range)
{
	if (m_childItems.count() == 0) {return;}

	/// the default behavior is to set ZDepthRanges to child items.
	double rangeWidth = range.width();
	double divNum = 0;
	divNum += m_childItems.count() - 1;
	for (GraphicsWindowDataItem* child : m_childItems) {
		int itemCount = (child->zDepthRange().itemCount() - 1);
		if (itemCount > 0) {
			divNum += itemCount;
		}
	}
	if (divNum == 0) {divNum = 1;}
	double divWidth = rangeWidth / divNum;
	double max = range.max();
	for (GraphicsWindowDataItem* child : m_childItems) {
		int itemCount = (child->zDepthRange().itemCount() - 1);
		int itemCount2 = 0;
		if (itemCount > 0) {
			itemCount2 = itemCount;
		}
		double min = max - itemCount2 * divWidth;
		if (min < range.min()) {min = range.min();}
		ZDepthRange r = child->zDepthRange();
		r.setMin(min);
		r.setMax(max);
		child->setZDepthRange(r);
		max = min - divWidth;
	}
}

QStringList GraphicsWindowDataItem::containedFiles()
{
	QStringList ret;
	ret << ProjectDataItem::containedFiles();
	for (GraphicsWindowDataItem* child : m_childItems) {
		ret << child->containedFiles();
	}
	return ret;
}

void GraphicsWindowDataItem::updateZDepthRangeItemCount()
{
	// update the ZDepthRange itemcount of child items first.
	int sum = 0;
	for (GraphicsWindowDataItem* child : m_childItems) {
		child->updateZDepthRangeItemCount();
		sum += child->zDepthRange().itemCount();
	}
	m_zDepthRange.setItemCount(sum);
}

void GraphicsWindowDataItem::update2Ds()
{
	innerUpdate2Ds();
	for (GraphicsWindowDataItem* child : m_childItems) {
		child->update2Ds();
	}
}

void GraphicsWindowDataItem::updateZScale(double scale)
{
	innerUpdateZScale(scale);
	for (GraphicsWindowDataItem* child : m_childItems) {
		child->updateZScale(scale);
	}
}

bool GraphicsWindowDataItem::hasTransparentPart()
{
	bool hasTransparent = myHasTransparentPart();
	for (GraphicsWindowDataItem* child : m_childItems) {
		hasTransparent = hasTransparent || child->hasTransparentPart();
	}
	return hasTransparent;
}

PostSolutionInfo* GraphicsWindowDataItem::postSolutionInfo()
{
	return projectData()->mainfile()->postSolutionInfo();
}

void GraphicsWindowDataItem::viewOperationEndedGlobal(VTKGraphicsView* v)
{
	doViewOperationEndedGlobal(v);
	for (GraphicsWindowDataItem* child : m_childItems) {
		child->viewOperationEndedGlobal(v);
	}
}

void GraphicsWindowDataItem::applyOffset(double x, double y)
{
	doApplyOffset(x, y);
	for (GraphicsWindowDataItem* child : m_childItems) {
		child->applyOffset(x, y);
	}
}

QVector2D GraphicsWindowDataItem::getOffset()
{
	GraphicsWindowDataItem* p = dynamic_cast<GraphicsWindowDataItem*>(parent());
	return p->getOffset();
}

void GraphicsWindowDataItem::clearChildItems()
{
	for (auto child : m_childItems) {
		delete child;
	}
	m_childItems.clear();
}

namespace {

class ModificationCommand : public QUndoCommand
{
public:
	explicit ModificationCommand(QUndoCommand *child, ProjectMainFile* main) :
		m_command {child},
		m_mainFile {main},
		m_wasModified {main->isModified()}
	{}
	~ModificationCommand()
	{}
	void undo() override
	{
		m_command.get()->undo();
		m_mainFile->setModified(m_wasModified);
	}
	void redo() override
	{
		m_command.get()->redo();
		m_mainFile->setModified();
	}

	int id() const
	{
		return m_command->id();
	}
	bool mergeWith(const QUndoCommand *other)
	{
		const ModificationCommand* modc = dynamic_cast<const ModificationCommand*> (other);
		if (modc == nullptr) {return false;}

		return m_command.get()->mergeWith(modc->m_command.get());
	}

private:
	std::unique_ptr<QUndoCommand> m_command;
	ProjectMainFile* m_mainFile;
	bool m_wasModified;
};


class StandardItemModifyCommand : public QUndoCommand
{
public:
	explicit StandardItemModifyCommand(QUndoCommand *child, GraphicsWindowDataItem* item) :
		QUndoCommand {child->text()},
		m_command {child},
		m_item {item}
	{}
	~StandardItemModifyCommand()
	{}
	void redo() override
	{
		m_item->setIsCommandExecuting(true);
		m_command.get()->redo();
		m_item->setIsCommandExecuting(false);
	}
	void undo() override
	{
		m_item->setIsCommandExecuting(true);
		m_command.get()->undo();
		m_item->setIsCommandExecuting(false);
	}
	int id() const
	{
		return m_command->id();
	}
	bool mergeWith(const QUndoCommand *other)
	{
		const StandardItemModifyCommand* modc = dynamic_cast<const StandardItemModifyCommand*> (other);
		if (modc == nullptr) {return false;}

		return m_command.get()->mergeWith(modc->m_command.get());
	}

private:
	std::unique_ptr<QUndoCommand> m_command;
	GraphicsWindowDataItem* m_item;
};

class RenderCommand : public QUndoCommand
{
public:
	explicit RenderCommand(QUndoCommand *child, GraphicsWindowDataItem* item) :
		QUndoCommand {child->text()},
		m_command {child},
		m_item {item}
	{}
	~RenderCommand()
	{}
	void undo() override
	{
		m_command.get()->undo();
		m_item->renderGraphicsView();
	}
	void redo() override
	{
		m_command.get()->redo();
		m_item->renderGraphicsView();
	}

	int id() const
	{
		return m_command->id();
	}
	bool mergeWith(const QUndoCommand *other)
	{
		const RenderCommand* modc = dynamic_cast<const RenderCommand*> (other);
		if (modc == nullptr) {return false;}

		return m_command.get()->mergeWith(modc->m_command.get());
	}

private:
	std::unique_ptr<QUndoCommand> m_command;
	GraphicsWindowDataItem* m_item;
};

}

void GraphicsWindowDataItem::pushCommand(QUndoCommand* com, GraphicsWindowDataItem* item)
{
	QUndoCommand* com2 = new ModificationCommand(com, projectData()->mainfile());
	if (item != nullptr) {
		com2 = new StandardItemModifyCommand(com2, item);
	}
	iRICUndoStack::instance().push(com2);
}

void GraphicsWindowDataItem::pushRenderCommand(QUndoCommand* com, GraphicsWindowDataItem *item, bool editItem)
{
	QUndoCommand* com2 = new RenderCommand(com, item);
	if (editItem) {
		pushCommand(com2, item);
	} else {
		pushCommand(com2);
	}
}
