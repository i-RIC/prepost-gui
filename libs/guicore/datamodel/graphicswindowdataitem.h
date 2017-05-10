#ifndef GRAPHICSWINDOWDATAITEM_H
#define GRAPHICSWINDOWDATAITEM_H

#include "../guicore_global.h"

#include "../project/projectdataitem.h"

#include <misc/zdepthrange.h>

#include <QStandardItem>
#include <QList>
#include <QVariant>

#include <vector>

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
class QUndoCommand;
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
	enum CheckFlag {Checked, NotChecked};
	enum DeleteFlag {Deletable, NotDeletable};
	enum ReorderFlag {Reorderable, NotReorderable};

	static const int dragUpdateRate = 10;
	/// Constructor
	GraphicsWindowDataItem(const QString& itemlabel, GraphicsWindowDataItem* parent);
	/// Constructor
	GraphicsWindowDataItem(const QString& itemlabel, const QIcon& icon, GraphicsWindowDataItem* parent);
	/// Constructor for root node.
	GraphicsWindowDataItem(ProjectDataItem* parent);
	virtual ~GraphicsWindowDataItem();
	bool isEnabled() const;
	void setEnabled(bool enabled);

	QMainWindow* mainWindow() const;
	/// Load data from project main file
	virtual void loadFromProjectMainFile(const QDomNode& node) override;
	/// Save data into project main file
	virtual void saveToProjectMainFile(QXmlStreamWriter& writer) override;
	/// The QStandardItem that corresponds to this item.
	QStandardItem* standardItem() const {return m_standardItem;}
	/// The QStandardItem that corresponds to this item.
	virtual void updateItemMap() {dynamic_cast<GraphicsWindowDataItem*>(parent())->updateItemMap();}
	virtual void updateExpandState(QTreeView* view);
	virtual void reflectExpandState(QTreeView* view);
	void innerUpdateItemMap(QMap<QStandardItem*, GraphicsWindowDataItem*>& map);
	/// Handle the event that the corresponding standardItem is changed.
	virtual void handleStandardItemChange();
	/// Handle the event that the corresponding standardItem is clicked.
	virtual void handleStandardItemClicked() {}
	/// Handle the event that the corresponding standardItem is double-clicked.
	virtual void handleStandardItemDoubleClicked() {}
	/// Load data from CGNS file
	virtual void loadFromCgnsFile(const int fn) override;
	/// Save data into CGNS file
	virtual void saveToCgnsFile(const int fn) override;
	/// Discard data loaded from CGNS file.
	virtual void closeCgnsFile() override;
	/// Returns true if this item is deletable.
	virtual bool isDeletable() const {return m_isDeletable;}
	/// Returns true if this item can be moved up or down.
	virtual bool isReorderable() const {return m_isReorderable;}
	virtual void addCustomMenuItems(QMenu* /*menu*/) {}
	/// This function is called when the corresponding node in object browser is selected.
	virtual void informSelection(VTKGraphicsView* /*v*/) {}
	/// This function is called when the corresponding node in object browser is deselected.
	virtual void informDeselection(VTKGraphicsView* /*v*/) {}
	virtual void viewOperationEnded(VTKGraphicsView* /*v*/) {}
	virtual void viewOperationEndedGlobal(VTKGraphicsView* /*v*/);
	virtual void keyPressEvent(QKeyEvent* /*event*/, VTKGraphicsView* /*v*/) {}
	virtual void keyReleaseEvent(QKeyEvent* /*event*/, VTKGraphicsView* /*v*/) {}
	virtual void mouseDoubleClickEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/) {}
	virtual void mouseMoveEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/) {}
	virtual void mousePressEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/) {}
	virtual void mouseReleaseEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/) {}
	virtual void wheelEvent(QWheelEvent* /*event*/, VTKGraphicsView* /*v*/) {}

	virtual bool hasTransparentPart();

	/// Returns the pointer to a dialog which shows the property edit dialog of this item.
	/**
	 * If no property dialog is available, this function returns 0.
	 */
	virtual QDialog* propertyDialog(QWidget* /*parent*/) {return 0;}
	virtual void handlePropertyDialogAccepted(QDialog* /*propDialog*/) {}

	/// Returns the pointer to a dialog which shows the add dialog of this item.
	/**
	 * If no add dialog is available, this function returns 0.
	 */
	virtual QDialog* addDialog(QWidget* /*parent*/) {return 0;}
	virtual void handleAddDialogAccepted(QDialog* /*addDialog*/) {}

	virtual void undoableDeleteItem(GraphicsWindowDataItem* /*item*/, bool /*noDraw*/ = false) {}

	/// Returns the pointer to a toolbar specific to currently selected item.
	virtual bool addToolBarButtons(QToolBar* /*parent*/) {return false;}
	const std::vector<GraphicsWindowDataItem*>& childItems() const {return m_childItems;}
	/// Move up the order in object browser.
	virtual void moveUp();
	/// Move down the order in object browser.
	virtual void moveDown();
	/// Z depth value range assigned for this item.
	virtual void updateZDepthRangeItemCount();
	const ZDepthRange& zDepthRange() const {return m_zDepthRange;}
	void setZDepthRange(const ZDepthRange& newrange);
	void updateZDepthRange();
	void startClosingProject();
	vtkActorCollection* actorCollection() const {return m_actorCollection;}
	vtkActor2DCollection* actor2DCollection() const {return m_actor2DCollection;}
	virtual QStringList containedFiles() override;
	/// Update the status (enabled or disabled) of move-up, and move-down actions.
	virtual void updateMoveUpDownActions(ObjectBrowserView* /*view*/) {}
	void setIsCommandExecuting(bool exec);
	void update2Ds();
	void updateZScale(double scale);
	void loadCheckState(const QDomNode& node);
	void renderGraphicsView();
	virtual void updateVisibilityWithoutRendering();
	void applyOffset(double x, double y);

public slots:
	virtual void showPropertyDialog();
	virtual void showAddDialog();

protected:
	void setupStandardItem(CheckFlag cflag, ReorderFlag rflag, DeleteFlag dflag, const QString& text = "");

	PostSolutionInfo* postSolutionInfo();
	virtual void assignActorZValues(const ZDepthRange& range);
	virtual GraphicsWindowDataModel* dataModel() const;
	vtkRenderer* renderer() const;
	virtual void unregisterChild(GraphicsWindowDataItem* child);
	/// Returns true when alal ancient nodes are checked.
	virtual bool isAncientChecked() const;
	/// Initialize itself
	virtual void init();
	virtual void updateVisibility();
	virtual void updateVisibility(bool visible);
	virtual void innerUpdate2Ds();
	virtual void innerUpdateZScale(double scale);
	virtual bool myHasTransparentPart() const;
	virtual void doViewOperationEndedGlobal(VTKGraphicsView* v);
	void pushCommand(QUndoCommand* com, GraphicsWindowDataItem *item = nullptr);
	void pushRenderCommand(QUndoCommand* com, GraphicsWindowDataItem *item, bool editItem = false);

	/// Build an instance of some class that inherits QGraphicsItem.
	void saveCheckState(QXmlStreamWriter& writer);
	void loadExpandState(const QDomNode& node);
	void saveExpandState(QXmlStreamWriter& writer);
	virtual QVector2D getOffset();
	/// Delete child items
	void clearChildItems();

	std::vector<GraphicsWindowDataItem*> m_childItems;
	QStandardItem* m_standardItem;
	QStandardItem* m_standardItemCopy {nullptr};
	bool m_isDeletable {true};
	bool m_isReorderable {false};
	bool m_isExpanded {false};
	vtkActorCollection* m_actorCollection;
	vtkActor2DCollection* m_actor2DCollection;
	ZDepthRange m_zDepthRange;
	bool m_isCommandExecuting {false};

protected:
	/// If true, the PreProcessorDataItem tree is under destruction.
	bool m_isDestructing {false};

public:
	friend class BackgroundImageInfo;
	friend class GraphicsWindowDrawOnRedo;
	friend class GraphicsWindowDrawOnUndo;
	friend class GraphicsWindowDataItemStandardItemChangeCommand;

private:
	class ModifyCommand;
	class RenderCommand;
	class StandardItemModifyCommand;
};

#endif // GRAPHICSWINDOWDATAITEM_H
