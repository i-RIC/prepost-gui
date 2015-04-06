#ifndef GRAPHICSWINDOWDATAITEM_H
#define GRAPHICSWINDOWDATAITEM_H

#include "../guicore_global.h"

#include "../project/projectdataitem.h"

#include <misc/zdepthrange.h>

#include <QUndoCommand>
#include <QStandardItem>

class QStandardItem;
class VTKGraphicsView;
class QIcon;
class QMenu;
class QDialog;
class QToolBar;
class QTreeView;
class QKeyEvent;
class QMainWindow;
class QMouseEvent;
class QWheelEvent;
class QXmlStreamWriter;

class vtkActorCollection;
class vtkActor2DCollection;
class vtkRenderer;
class GraphicsWindowDataModel;
class ObjectBrowserView;
class GraphicsWindowDataItemStandardItemChangeCommand;
class BackgroundImageInfo;
class PostSolutionInfo;

class GUICOREDLL_EXPORT GraphicsWindowDataItem : public ProjectDataItem
{
public:
	static const int dragUpdateRate = 10;
	/// Constructor
	GraphicsWindowDataItem(const QString& itemlabel, GraphicsWindowDataItem* parent);
	/// Constructor
	GraphicsWindowDataItem(const QString& itemlabel, const QIcon& icon, GraphicsWindowDataItem* parent);
	/// Constructor for root node.
	GraphicsWindowDataItem(ProjectDataItem* parent);
	virtual ~GraphicsWindowDataItem();
	bool isEnabled();
	void setEnabled(bool enabled);

	QMainWindow* mainWindow();
	/// Load data from project main file
	virtual void loadFromProjectMainFile(const QDomNode& node);
	/// Save data into project main file
	virtual void saveToProjectMainFile(QXmlStreamWriter& writer);
	/// The QStandardItem that corresponds to this item.
	QStandardItem* standardItem(){return m_standardItem;}
	/// The QStandardItem that corresponds to this item.
	virtual void updateItemMap(){dynamic_cast<GraphicsWindowDataItem*>(parent())->updateItemMap();}
	virtual void updateExpandState(QTreeView* view);
	virtual void reflectExpandState(QTreeView* view);
	void innerUpdateItemMap(QMap<QStandardItem*, GraphicsWindowDataItem*>& map);
	/// Handle the event that the corresponding standardItem is changed.
	virtual void handleStandardItemChange();
	/// Handle the event that the corresponding standardItem is clicked.
	virtual void handleStandardItemClicked(){}
	/// Handle the event that the corresponding standardItem is double-clicked.
	virtual void handleStandardItemDoubleClicked(){}
	/// Load data from CGNS file
	virtual void loadFromCgnsFile(const int fn);
	/// Save data into CGNS file
	virtual void saveToCgnsFile(const int fn);
	/// Discard data loaded from CGNS file.
	virtual void closeCgnsFile();
	/// Returns true if this item is deletable.
	virtual bool isDeletable(){return m_isDeletable;}
	/// Returns true if this item can be moved up or down.
	virtual bool isReorderable(){return m_isReorderable;}
	virtual void addCustomMenuItems(QMenu* /*menu*/){}
	/// This function is called when the corresponding node in object browser is selected.
	virtual void informSelection(VTKGraphicsView * /*v*/){}
	/// This function is called when the corresponding node in object browser is deselected.
	virtual void informDeselection(VTKGraphicsView* /*v*/){}
	virtual void viewOperationEnded(VTKGraphicsView* /*v*/){}
	virtual void viewOperationEndedGlobal(VTKGraphicsView* /*v*/);
	virtual void keyPressEvent(QKeyEvent* /*event*/, VTKGraphicsView* /*v*/){}
	virtual void keyReleaseEvent(QKeyEvent* /*event*/, VTKGraphicsView* /*v*/){}
	virtual void mouseDoubleClickEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/){}
	virtual void mouseMoveEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/){}
	virtual void mousePressEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/){}
	virtual void mouseReleaseEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/){}
	virtual void wheelEvent(QWheelEvent* /*event*/, VTKGraphicsView* /*v*/){}

	virtual bool hasTransparentPart();

	virtual void showPropertyDialog();
	/// Returns the pointer to a dialog which shows the property edit dialog of this item.
	/**
	 * If no property dialog is available, this function returns 0.
	 */
	virtual QDialog* propertyDialog(QWidget* /*parent*/){return 0;}
	virtual void handlePropertyDialogAccepted(QDialog* /*propDialog*/){}
	/// Returns the pointer to a toolbar specific to currently selected item.
	virtual bool addToolBarButtons(QToolBar* /*parent*/){return false;}
	const QList <GraphicsWindowDataItem*>& childItems() const {return m_childItems;}
	/// Move up the order in object browser.
	virtual void moveUp();
	/// Move down the order in object browser.
	virtual void moveDown();
	/// Z depth value range assigned for this item.
	virtual void updateZDepthRangeItemCount();
	ZDepthRange zDepthRange(){return m_zDepthRange;}
	void setZDepthRange(const ZDepthRange& newrange);
	void updateZDepthRange();
	void startClosingProject();
	vtkActorCollection* actorCollection(){return m_actorCollection;}
	vtkActor2DCollection* actor2DCollection(){return m_actor2DCollection;}
	virtual QStringList containedFiles();
	/// Update the status (enabled or disabled) of move-up, and move-down actions.
	virtual void updateMoveUpDownActions(ObjectBrowserView* /*view*/){}
	void setIsCommandExecuting(bool exec){m_isCommandExecuting = exec;}
	void update2Ds();
	void updateZScale(double scale);
	void loadCheckState(const QDomNode& node);
	void renderGraphicsView();
	virtual void updateVisibilityWithoutRendering();
	void applyOffset(double x, double y);

protected:
	PostSolutionInfo* postSolutionInfo();
	virtual void assignActionZValues(const ZDepthRange& range);
	virtual GraphicsWindowDataModel* dataModel(){return dynamic_cast<GraphicsWindowDataItem*>(parent())->dataModel();}
	vtkRenderer* renderer();
	virtual void unregisterChild(GraphicsWindowDataItem* child);
	/// Returns true when alal ancient nodes are checked.
	virtual bool isAncientChecked();
	/// Initialize itself
	virtual void init();
	virtual void updateVisibility();
	virtual void updateVisibility(bool visible);
	virtual void innerUpdate2Ds(){}
	virtual void innerUpdateZScale(double /*scale*/){}
	virtual bool myHasTransparentPart(){return false;}
	virtual void doViewOperationEndedGlobal(VTKGraphicsView* /*v*/){}

	/// Build an instance of some class that inherits QGraphicsItem.
	void saveCheckState(QXmlStreamWriter& writer);
	void loadExpandState(const QDomNode& node);
	void saveExpandState(QXmlStreamWriter& writer);
	virtual QVector2D getOffset();
	QList <GraphicsWindowDataItem*> m_childItems;
	QStandardItem* m_standardItem;
	QStandardItem* m_standardItemCopy;
	bool m_isDeletable;
	bool m_isReorderable;
	bool m_isExpanded;
	vtkActorCollection* m_actorCollection;
	vtkActor2DCollection* m_actor2DCollection;
	ZDepthRange m_zDepthRange;
	bool m_isCommandExecuting;

protected:
	/// If true, the PreProcessorDataItem tree is under destruction.
	bool m_isDestructing;
	bool m_isPushing;

public:
	friend class BackgroundImageInfo;
	friend class GraphicsWindowDrawOnRedo;
	friend class GraphicsWindowDrawOnUndo;
	friend class GraphicsWindowDataItemStandardItemChangeCommand;
};

/// The class to store standard item change information.
/**
	* This class stores only displayRole data and checkState data.
	*/
class GraphicsWindowDataItemStandardItemChangeCommand : public QUndoCommand
{
public:
	GraphicsWindowDataItemStandardItemChangeCommand(GraphicsWindowDataItem* item)
		: QUndoCommand(QObject::tr("Object Browser Item Change"))
	{
		m_oldDisplayText = item->m_standardItemCopy->data(Qt::DisplayRole);
		m_oldCheckState = item->m_standardItemCopy->data(Qt::CheckStateRole);

		m_newDisplayText = item->m_standardItem->data(Qt::DisplayRole);
		m_newCheckState = item->m_standardItem->data(Qt::CheckStateRole);

		m_item = item;
	}

	void redoStandardItem(){
		m_item->m_standardItemCopy->setData(m_newDisplayText, Qt::DisplayRole);
		m_item->m_standardItemCopy->setData(m_newCheckState, Qt::CheckStateRole);

		m_item->m_standardItem->setData(m_newDisplayText, Qt::DisplayRole);
		m_item->m_standardItem->setData(m_newCheckState, Qt::CheckStateRole);

		m_item->setModified();
	}

	void redo()
	{
		m_item->setIsCommandExecuting(true);

		redoStandardItem();

		m_item->updateVisibility();
		m_item->setIsCommandExecuting(false);
	}

	void undoStandardItem(){
		m_item->m_standardItemCopy->setData(m_oldDisplayText, Qt::DisplayRole);
		m_item->m_standardItemCopy->setData(m_oldCheckState, Qt::CheckStateRole);

		m_item->m_standardItem->setData(m_oldDisplayText, Qt::DisplayRole);
		m_item->m_standardItem->setData(m_oldCheckState, Qt::CheckStateRole);

		m_item->setModified();
	}
	void undo()
	{
		m_item->setIsCommandExecuting(true);

		undoStandardItem();

		m_item->updateVisibility();
		m_item->setIsCommandExecuting(false);
	}
private:
	QVariant m_oldDisplayText;
	QVariant m_oldCheckState;

	QVariant m_newDisplayText;
	QVariant m_newCheckState;
	GraphicsWindowDataItem* m_item;
};

#endif // GRAPHICSWINDOWDATAITEM_H
