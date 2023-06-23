#ifndef GRAPHICSWINDOWDATAITEM_H
#define GRAPHICSWINDOWDATAITEM_H

#include "../guicore_global.h"

#include "../project/projectdataitem.h"

#include <misc/zdepthrange.h>

#include <vector>

class GraphicsWindowDataModel;
class ObjectBrowserView;
class PostSolutionInfo;
class VTKGraphicsView;

class QDialog;
class QIcon;
class QKeyEvent;
class QMainWindow;
class QMenu;
class QMouseEvent;
class QResizeEvent;
class QStandardItem;
class QToolBar;
class QTreeView;
class QUndoCommand;
class QWheelEvent;
class QXmlStreamWriter;

class vtkActorCollection;
class vtkActor2DCollection;
class vtkRenderer;

class GUICOREDLL_EXPORT GraphicsWindowDataItem : public ProjectDataItem
{

public:
	enum CheckFlag {Checked, NotChecked};
	enum DeleteFlag {Deletable, NotDeletable};
	enum ReorderFlag {Reorderable, NotReorderable};

	static const int dragUpdateRate = 10;

	GraphicsWindowDataItem(const QString& itemlabel, GraphicsWindowDataItem* parent);
	GraphicsWindowDataItem(const QString& itemlabel, const QIcon& icon, GraphicsWindowDataItem* parent);
	GraphicsWindowDataItem(ProjectDataItem* parent);
	virtual ~GraphicsWindowDataItem();

	bool isEnabled() const;
	void setEnabled(bool enabled);

	bool isChecked() const;
	virtual bool isAncientChecked() const;

	QMainWindow* mainWindow() const;

	virtual void loadFromProjectMainFile(const QDomNode& node) override;
	virtual void saveToProjectMainFile(QXmlStreamWriter& writer) override;
	QStandardItem* standardItem() const;
	virtual void updateItemMap();
	virtual void updateExpandState(QTreeView* view);
	virtual void reflectExpandState(QTreeView* view);
	void innerUpdateItemMap(QMap<QStandardItem*, GraphicsWindowDataItem*>& map);
	virtual void handleStandardItemChange();
	virtual void handleStandardItemDoubleClicked() {}
	/// Load data from CGNS file
	int loadFromCgnsFile() override;
	/// Save data into CGNS file
	int saveToCgnsFile() override;
	int updateCgnsFileOtherThanGrids() override;
	/// Discard data loaded from CGNS file.
	virtual void closeCgnsFile() override;
	virtual bool isDeletable() const;
	virtual bool isReorderable() const;
	virtual void addCustomMenuItems(QMenu* menu);
	virtual void informSelection(VTKGraphicsView* v);
	virtual void informDeselection(VTKGraphicsView* v);
	virtual void viewOperationEnded(VTKGraphicsView* v);
	virtual void viewOperationEndedGlobal(VTKGraphicsView* v);
	virtual void handleResize(QResizeEvent* event, VTKGraphicsView* v);
	virtual void keyPressEvent(QKeyEvent* event, VTKGraphicsView* v);
	virtual void keyReleaseEvent(QKeyEvent* event, VTKGraphicsView* v);
	virtual void mouseDoubleClickEvent(QMouseEvent* event, VTKGraphicsView* v);
	virtual void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v);
	virtual void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v);
	virtual void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v);
	virtual void wheelEvent(QWheelEvent* event, VTKGraphicsView* v);
	virtual bool hasTransparentPart();

	virtual QDialog* propertyDialog(QWidget* parent);
	virtual void handlePropertyDialogAccepted(QDialog* propDialog);

	virtual QDialog* addDialog(QWidget* parent);
	virtual void handleAddDialogAccepted(QDialog* addDialog);

	virtual bool addToolBarButtons(QToolBar* parent);
	const std::vector<GraphicsWindowDataItem*>& childItems() const;

	/// Move up the order in object browser.
	virtual void moveUp();
	/// Move down the order in object browser.
	virtual void moveDown();
	/// Z depth value range assigned for this item.
	virtual void updateZDepthRangeItemCount();
	const ZDepthRange& zDepthRange() const;
	void setZDepthRange(const ZDepthRange& newrange);
	void updateZDepthRange();
	void startClosingProject();
	vtkActorCollection* actorCollection() const;
	vtkActor2DCollection* actor2DCollection() const;
	QStringList containedFiles() const override;
	/// Update the status (enabled or disabled) of move-up, and move-down actions.
	virtual void updateMoveUpDownActions(ObjectBrowserView* view);
	void setIsCommandExecuting(bool exec);
	void update2Ds();
	void updateZScale(double scale);
	void loadCheckState(const QDomNode& node);
	void renderGraphicsView();
	virtual void updateVisibilityWithoutRendering();
	void applyOffset(double x, double y);
	void pushCommand(QUndoCommand* com, GraphicsWindowDataItem *item = nullptr);
	void pushRenderCommand(QUndoCommand* com, GraphicsWindowDataItem *item, bool editItem = false);
	void pushRenderRedoOnlyCommand(QUndoCommand* com, GraphicsWindowDataItem *item, bool editItem = false);
	void pushUpdateActorSettingCommand(QUndoCommand* com, GraphicsWindowDataItem* item, bool editItem = false);

public slots:
	virtual void showPropertyDialog();
	virtual void showAddDialog();

protected:
	void setupStandardItem(CheckFlag cflag, ReorderFlag rflag, DeleteFlag dflag, const QString& text = "");

	PostSolutionInfo* postSolutionInfo();
	virtual void assignActorZValues(const ZDepthRange& range);
	void assignActorZValues(const ZDepthRange& range, const std::vector<GraphicsWindowDataItem*>& items);
	virtual GraphicsWindowDataModel* dataModel() const;
	vtkRenderer* renderer() const;
	virtual void unregisterChild(GraphicsWindowDataItem* child);
	/// Initialize itself
	virtual void init();
	virtual void updateActorSetting();
	virtual void updateVisibility();
	virtual void updateVisibility(bool visible);
	virtual void innerUpdate2Ds();
	virtual void innerUpdateZScale(double scale);
	virtual bool myHasTransparentPart() const;
	virtual void doViewOperationEndedGlobal(VTKGraphicsView* v);
	virtual void doHandleResize(QResizeEvent* event, VTKGraphicsView* v);

	/// Build an instance of some class that inherits QGraphicsItem.
	void saveCheckState(QXmlStreamWriter& writer);
	void loadExpandState(const QDomNode& node);
	void saveExpandState(QXmlStreamWriter& writer);
	virtual QPointF getOffset();
	void addChildItem(GraphicsWindowDataItem* child);
	void clearChildItems();

	void showPropertyDialogModal();
	void showPropertyDialogModeless();

	std::vector<GraphicsWindowDataItem*> m_childItems;
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
	bool m_isDestructing {false};

public:
	friend class BackgroundImageInfo;
	friend class GraphicsWindowDrawOnRedo;
	friend class GraphicsWindowDrawOnUndo;
	friend class GraphicsWindowDataItemStandardItemChangeCommand;

private:
	class ModifyCommand;
	class RenderCommand;
	class RenderRedoOnlyCommand;
	class StandardItemModifyCommand;
	class UpdateActorSettingCommand;
};

#endif // GRAPHICSWINDOWDATAITEM_H
