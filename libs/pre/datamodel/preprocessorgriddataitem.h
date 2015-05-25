#ifndef PREPROCESSORGRIDDATAITEM_H
#define PREPROCESSORGRIDDATAITEM_H

#include <guicore/pre/base/preprocessorgriddataiteminterface.h>
#include <misc/edge.h>
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkMapper.h>
#include <vtkUnstructuredGrid.h>
#include <vtkDataSetMapper.h>
#include <vtkExtractCells.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>

#include <QVector>

class Grid;
class QSignalMapper;

class QAction;
class PreProcessorGridShapeDataItem;
class PreProcessorGridRelatedConditionNodeGroupDataItem;
class PreProcessorGridRelatedConditionNodeDataItem;
class PreProcessorGridRelatedConditionCellGroupDataItem;
class PreProcessorGridRelatedConditionCellDataItem;
class PreProcessorBCGroupDataItem;
class PreProcessorBCDataItem;

class GridPointMouseMoveCommand;
class GridRelatedConditionEditCommand;
class GridBirdEyeWindow;

class PreProcessorGridDataItem : public PreProcessorGridDataItemInterface
{
	Q_OBJECT
public:
	/// Constructor
	PreProcessorGridDataItem(PreProcessorDataItem* parent);
	~PreProcessorGridDataItem();
	Grid* grid() const override {return m_grid;}
	bool setGrid(Grid* newGrid) override;
	void loadFromCgnsFile(const int fn) override;
	void saveToCgnsFile(const int fn) override;
	void closeCgnsFile() override;
	void addCustomMenuItems(QMenu* menu) override;

	// Standard mouse event handlers
	void mouseDoubleClickEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/) override;
	void mouseMoveEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/) override;
	void mousePressEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/) override;
	void mouseReleaseEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/) override;

	// Mouse event handlers for grid nodes selecting

	void nodeSelectingMouseMoveEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/);
	void nodeSelectingMousePressEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/);
	void nodeSelectingMouseReleaseEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/);

	void nodeSelectingKeyPressEvent(QKeyEvent* /*event*/, VTKGraphicsView* /*v*/);
	void nodeSelectingKeyReleaseEvent(QKeyEvent* /*event*/, VTKGraphicsView* /*v*/);

	// Mouse event handlers for grid cells selecting

	void cellSelectingMouseMoveEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/);
	void cellSelectingMousePressEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/);
	void cellSelectingMouseReleaseEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/);

	void cellSelectingKeyPressEvent(QKeyEvent* event, VTKGraphicsView* v) {
		nodeSelectingKeyPressEvent(event, v);
	}

	void cellSelectingKeyReleaseEvent(QKeyEvent* event, VTKGraphicsView* v) {
		nodeSelectingKeyReleaseEvent(event, v);
	}

	// Mouse event handlers for grid edges selecting

	void edgeSelectingMouseMoveEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/);
	void edgeSelectingMousePressEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/);
	void edgeSelectingMouseReleaseEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/);

	void edgeSelectingKeyPressEvent(QKeyEvent* event, VTKGraphicsView* v) {
		nodeSelectingKeyPressEvent(event, v);
	}

	void edgeSelectingKeyReleaseEvent(QKeyEvent* event, VTKGraphicsView* v) {
		nodeSelectingKeyReleaseEvent(event, v);
	}

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;

	void setSelectedPointsVisibility(bool visible);
	void setSelectedCellsVisibility(bool visible);
	void setSelectedEdgesVisibility(bool visible);

	void updateZDepthRangeItemCount() override;
	void informgridRelatedConditionChangeAll();
	void informgridRelatedConditionChange(const QString& name) override;
	void silentDeleteGrid() override;
	PreProcessorGridShapeDataItem* shapeDataItem() {return m_shapeDataItem;}
	PreProcessorGridRelatedConditionNodeGroupDataItem* nodeGroupDataItem() {return m_nodeGroupDataItem;}
	PreProcessorGridRelatedConditionCellGroupDataItem* cellGroupDataItem() {return m_cellGroupDataItem;}
	PreProcessorBCGroupDataItem* bcGroupDataItem() {return m_bcGroupDataItem;}
	void setSelectedVertices(const QVector<vtkIdType>& vertices);
	QVector<vtkIdType> getCellsFromVertices(const QSet<vtkIdType>& vertices) const;
	QVector<Edge> getEdgesFromVertices(const QSet<vtkIdType>& vertices) const;

	bool isImportAvailable();
	bool isExportAvailable();
	QAction* importAction() {return m_importAction;}
	QAction* exportAction() {return m_exportAction;}
	QAction* displaySettingAction() {return m_displaySettingAction;}
	QAction* deleteAction() {return m_deleteAction;}
	QAction* polygonSelectAction() {return m_polygonSelectAction;}
	QAction* mappingAction();
	QAction* nodeEditAction() {return m_nodeEditAction;}
	QAction* nodeDisplaySettingAction() {return m_nodeDisplaySettingAction;}
	QAction* cellEditAction() {return m_cellEditAction;}
	QAction* cellDisplaySettingAction() {return m_cellDisplaySettingAction;}
	QAction* setupScalarBarAction() {return m_setupScalarBarAction;}
	QAction* birdEyeWindowAction() {return m_birdEyeWindowAction;}

	QMenu* menu() {return m_menu;}
	vtkPolyData* selectedVerticesPolyData() const {return m_selectedVerticesPolyData;}
	const QVector<vtkIdType>& selectedVertices() const {return m_selectedVertices;}
	const QVector<vtkIdType>& selectedCells() const {return m_selectedCells;}
	const QVector<Edge>& selectedEdges() const {return m_selectedEdges;}
	void updateAttributeActorSettings() override;

	void setNodeDataItem(PreProcessorGridRelatedConditionNodeDataItem* nodeItem) {m_nodeDataItem = nodeItem;}
	void setCellDataItem(PreProcessorGridRelatedConditionCellDataItem* cellItem) {m_cellDataItem = cellItem;}
	QCursor normalCursor();
	void informGridChange();
	void informBirdEyeWindowClose();
	void doViewOperationEndedGlobal(VTKGraphicsView*) override;
	void updateSimplifiedGrid(VTKGraphicsView* v = nullptr) override;
	void updateRegionPolyData();

	void updateObjectBrowserTree();
signals:
	void gridRelatedConditionChanged(const QString& name);
public slots:
	void importGrid();
	void exportGrid();
private slots:
	void showDisplaySettingDialog();
	void deleteGrid();
	void openBirdEyeWindow();
private:
	vtkPolyData* buildEdges() const;
	void setupActors();
	void setupActions();
	void clearSelection();
	void updateSelectedVertices(MouseBoundingBox* box, bool xOr);
	QVector<vtkIdType> getSelectedVertices(MouseBoundingBox* box, bool xOr);
	QSet<vtkIdType> getSelectedVerticesSet(MouseBoundingBox* box, bool xOr);
	void updateSelectedCells(MouseBoundingBox* box, bool xOr);
	void updateSelectedEdges(MouseBoundingBox* box, bool xOr, VTKGraphicsView* v);
	void updateSelectedVerticesGraphics();
	void updateSelectedCellsGraphics();
	void updateSelectedEdgesGraphics();
	void finishGridLoading();
	void closeBirdEyeWindow();
	void informSelectedVerticesChanged();
protected:
	void renderGraphicsView() {GraphicsWindowDataItem::renderGraphicsView();}
	void doApplyOffset(double x, double y) override;
	virtual void setupMenu() = 0;
	virtual void updateActionStatus();
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void assignActorZValues(const ZDepthRange& range) override;


	vtkSmartPointer<vtkPolyData> m_regionPolyData;
	vtkSmartPointer<vtkPolyDataMapper> m_regionMapper;
	vtkSmartPointer<vtkActor> m_regionActor;

	vtkSmartPointer<vtkPolyData> m_selectedVerticesPolyData;
	vtkSmartPointer<vtkPolyDataMapper> m_selectedVerticesMapper;
	vtkSmartPointer<vtkActor> m_selectedVerticesActor;

	vtkSmartPointer<vtkExtractCells> m_selectedCellsGrid;
	vtkSmartPointer<vtkDataSetMapper> m_selectedCellsMapper;
	vtkSmartPointer<vtkActor> m_selectedCellsActor;

	vtkSmartPointer<vtkDataSetMapper> m_selectedCellsLinesMapper;
	vtkSmartPointer<vtkActor> m_selectedCellsLinesActor;

	vtkSmartPointer<vtkPolyData> m_selectedEdgesPolyData;
	vtkSmartPointer<vtkPolyDataMapper> m_selectedEdgesMapper;
	vtkSmartPointer<vtkActor> m_selectedEdgesActor;

	QMenu* m_menu;
	Grid* m_grid;
	PreProcessorGridShapeDataItem* m_shapeDataItem;
	PreProcessorGridRelatedConditionNodeGroupDataItem* m_nodeGroupDataItem;
	PreProcessorGridRelatedConditionCellGroupDataItem* m_cellGroupDataItem;
	PreProcessorBCGroupDataItem* m_bcGroupDataItem;
	QAction* m_importAction;
	QAction* m_exportAction;

	QAction* m_displaySettingAction;
	QAction* m_polygonSelectAction;
	QAction* m_deleteAction;

	QAction* m_nodeEditAction;
	QAction* m_nodeDisplaySettingAction;
	QAction* m_cellEditAction;
	QAction* m_cellDisplaySettingAction;
	QAction* m_birdEyeWindowAction;

	QAction* m_setupScalarBarAction;

	QVector<vtkIdType> m_selectedVertices;
	QVector<vtkIdType> m_selectedCells;
	QVector<Edge> m_selectedEdges;

	PreProcessorGridRelatedConditionNodeDataItem* m_nodeDataItem;
	PreProcessorGridRelatedConditionCellDataItem* m_cellDataItem;
private:
	GridBirdEyeWindow* m_birdEyeWindow;

	QPixmap m_addPixmap;
	QCursor m_addCursor;

	bool m_shiftPressed;
public:
	friend class GridPointMouseMoveCommand;
	friend class GridRelatedConditionEditCommand;
};

#endif // PREPROCESSORGRIDDATAITEM_H
