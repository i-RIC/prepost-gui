#ifndef GRAPH2DWINDOWDATAITEM_H
#define GRAPH2DWINDOWDATAITEM_H

#include "graph2d_global.h"
#include <guicore/project/projectdataitem.h>
#include <misc/zdepthrange.h>

#include <QXmlStreamReader>
#include <QXmlStreamWriter>

class QStandardItem;
class QMainWindow;
class QIcon;
class QMenu;
class QDialog;
class QToolBar;
class QTreeView;
class QKeyEvent;
class QMouseEvent;
class QWheelEvent;
class Graph2dWindowDataModel;
class ObjectBrowserView;
class Graph2dWindowDataItemStandardItemChangeCommand;

class GRAPH2D_EXPORT Graph2dWindowDataItem : public ProjectDataItem
{
public:
	static const int dragUpdateRate = 10;
	/// Constructor
	Graph2dWindowDataItem(const QString& itemlabel, Graph2dWindowDataItem* parent);
	/// Constructor
	Graph2dWindowDataItem(const QString& itemlabel, const QIcon& icon, Graph2dWindowDataItem* parent);
	/// Constructor for root node.
	Graph2dWindowDataItem(ProjectDataItem* parent);
	virtual ~Graph2dWindowDataItem();
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
	virtual void updateItemMap(){dynamic_cast<Graph2dWindowDataItem*>(parent())->updateItemMap();}
	virtual void updateExpandState(QTreeView* view);
	virtual void reflectExpandState(QTreeView* view);
	void innerUpdateItemMap(QMap<QStandardItem*, Graph2dWindowDataItem*>& map);
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

	void showPropertyDialog();
	/// Returns the pointer to a dialog which shows the property edit dialog of this item.
	/**
	 * If no property dialog is available, this function returns 0.
	 */
	virtual QDialog* propertyDialog(QWidget* /*parent*/){return 0;}
	virtual void handlePropertyDialogAccepted(QDialog* /*propDialog*/){}
	/// Returns the pointer to a toolbar specific to currently selected item.
	virtual bool addToolBarButtons(QToolBar* /*parent*/){return false;}
	const QList <Graph2dWindowDataItem*>& childItems() const {return m_childItems;}
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
	virtual QStringList containedFiles();
	/// Update the status (enabled or disabled) of move-up, and move-down actions.
	virtual void updateMoveUpDownActions(ObjectBrowserView* /*view*/){}
	void setIsCommandExecuting(bool exec){m_isCommandExecuting = exec;}
protected:
	virtual void assignActionZValues(const ZDepthRange& range);
	virtual Graph2dWindowDataModel* dataModel(){return dynamic_cast<Graph2dWindowDataItem*>(parent())->dataModel();}
	void renderView();
	virtual void unregisterChild(Graph2dWindowDataItem* child);
	/// Returns true when alal ancient nodes are checked.
	virtual bool isAncientChecked();
	/// Initialize itself
	virtual void init();
	virtual void updateVisibility();
	virtual void updateVisibility(bool visible);
	virtual void setVisible(bool /*visible*/){}
	/// Build an instance of some class that inherits QGraphicsItem.
	void loadCheckState(const QDomNode& node);
	void saveCheckState(QXmlStreamWriter& writer);
	void loadExpandState(const QDomNode& node);
	void saveExpandState(QXmlStreamWriter& writer);
	QList <Graph2dWindowDataItem*> m_childItems;
	QStandardItem* m_standardItem;
	QStandardItem* m_standardItemCopy;
	bool m_isDeletable;
	bool m_isReorderable;
	bool m_isExpanded;
	ZDepthRange m_zDepthRange;
	bool m_isCommandExecuting;
protected:
	/// If true, the PreProcessorDataItem tree is under destruction.
	bool m_isDestructing;
	bool m_isPushing;

public:
	friend class BackgroundImageInfo;
	friend class Graph2dWindowDrawOnRedo;
	friend class Graph2dWindowDrawOnUndo;
	friend class Graph2dWindowDataItemStandardItemChangeCommand;
};

#endif // GRAPH2DWINDOWDATAITEM_H
