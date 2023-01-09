#ifndef PREPROCESSORGRIDDATAITEM_H
#define PREPROCESSORGRIDDATAITEM_H

#include <guicore/pre/base/preprocessorgriddataiteminterface.h>
#include <misc/edge.h>

#include <QVector>

class Grid;
class QSignalMapper;

class QAction;
class PreProcessorGridAndGridCreatingConditionDataItem;
class PreProcessorGridShapeDataItem;
class PreProcessorGridAttributeNodeGroupDataItem;
class PreProcessorGridAttributeNodeDataItem;
class PreProcessorGridAttributeCellGroupDataItem;
class PreProcessorGridAttributeCellDataItem;
class PreProcessorBCGroupDataItem;

class GridPointMouseMoveCommand;
class GridAttributeEditCommand;

class vtkPolyData;

namespace iRICLib {
	class H5CgnsBase;
	class H5CgnsZone;
} // namespace iRICLib

class PreProcessorGridDataItem : public PreProcessorGridDataItemInterface
{
	Q_OBJECT

public:
	PreProcessorGridDataItem(PreProcessorDataItem* parent);
	~PreProcessorGridDataItem();

	Grid* grid() const override;
	bool setGrid(Grid* newGrid) override;

	int loadFromCgnsFile() override;
	int loadFromCgnsFile(const iRICLib::H5CgnsZone& zone);
	int saveToCgnsFile() override;
	int saveToCgnsFile(iRICLib::H5CgnsBase* base, const std::string& zoneName);
	void closeCgnsFile() override;

	void updateActionStatus() override;

	void addCustomMenuItems(QMenu* menu) override;

	// Standard mouse event handlers
	void mouseDoubleClickEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;

	// Mouse event handlers for grid nodes selecting

	void nodeSelectingMouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v);
	void nodeSelectingMousePressEvent(QMouseEvent* event, VTKGraphicsView* v);
	void nodeSelectingMouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v);

	void nodeSelectingKeyPressEvent(QKeyEvent* event, VTKGraphicsView* v);
	void nodeSelectingKeyReleaseEvent(QKeyEvent* event, VTKGraphicsView* v);

	// Mouse event handlers for grid cells selecting

	void cellSelectingMouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v);
	void cellSelectingMousePressEvent(QMouseEvent* event, VTKGraphicsView* v);
	void cellSelectingMouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v);

	void cellSelectingKeyPressEvent(QKeyEvent* event, VTKGraphicsView* v);
	void cellSelectingKeyReleaseEvent(QKeyEvent* event, VTKGraphicsView* v);

	// Mouse event handlers for grid edges selecting

	void edgeSelectingMouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v);
	void edgeSelectingMousePressEvent(QMouseEvent* event, VTKGraphicsView* v);
	void edgeSelectingMouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v);

	void edgeSelectingKeyPressEvent(QKeyEvent* event, VTKGraphicsView* v);
	void edgeSelectingKeyReleaseEvent(QKeyEvent* event, VTKGraphicsView* v);

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;

	void setSelectedPointsVisibility(bool visible);
	void setSelectedCellsVisibility(bool visible);
	void setSelectedEdgesVisibility(bool visible);

	void updateZDepthRangeItemCount() override;
	void informGridAttributeChangeAll();
	void informGridAttributeChange(const std::string& name) override;
	void silentDeleteGrid() override;

	PreProcessorGridShapeDataItem* shapeDataItem() const;
	PreProcessorGridAttributeNodeGroupDataItem* nodeGroupDataItem() const;
	PreProcessorGridAttributeCellGroupDataItem* cellGroupDataItem() const;
	PreProcessorBCGroupDataItem* bcGroupDataItem() const;

	void setSelectedVertices(const QVector<vtkIdType>& vertices);
	QVector<vtkIdType> getCellsFromVertices(const QSet<vtkIdType>& vertices) const;
	QVector<Edge> getEdgesFromVertices(const QSet<vtkIdType>& vertices) const;
	void setBCGroupDataItem(PreProcessorBCGroupDataItem* item) override;
	void unsetBCGroupDataItem() override;

	bool isImportAvailable();
	bool isExportAvailable();

	bool gridIsDeleted() const;

	QAction* importAction() const;
	QAction* exportAction() const;
	QAction* displaySettingAction() const;
	QAction* deleteAction() const;
	QAction* polygonSelectAction() const;
	QAction* mappingAction() const;
	QAction* nodeEditAction() const;
	QAction* nodeDisplaySettingAction() const;
	QAction* cellEditAction() const;
	QAction* cellDisplaySettingAction() const;
	QAction* setupScalarBarAction() const;
	QAction* birdEyeWindowAction() const;
	QMenu* generateAttMenu() const;
	QMenu* menu() const;

	vtkPolyData* selectedVerticesPolyData() const;
	const QVector<vtkIdType>& selectedVertices() const;
	const QVector<vtkIdType>& selectedCells() const;
	const QVector<Edge>& selectedEdges() const;
	void updateAttributeActorSettings() override;

	void setNodeDataItem(PreProcessorGridAttributeNodeDataItem* nodeItem);
	void setCellDataItem(PreProcessorGridAttributeCellDataItem* cellItem);
	QCursor normalCursor();
	void informGridChange();
	void informBirdEyeWindowClose();
	void doViewOperationEndedGlobal(VTKGraphicsView*) override;
	void updateSimplifiedGrid(VTKGraphicsView* v = nullptr) override;
	void updateRegionPolyData();

	void updateObjectBrowserTree();

signals:
	void gridAttributeChanged(const std::string& name);

public slots:
	void exportGrid();

private slots:
	void showDisplaySettingDialog();
	void deleteGrid();
	void openBirdEyeWindow();
	void launchAttributeGenerator();

private:
	vtkPolyData* buildEdges() const;
	void setupActors();
	void setupActions();
	void setupGenerateAttributeActions(QMenu* menu);
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
	PreProcessorGridShapeDataItem* m_shapeDataItem;
	PreProcessorGridAttributeNodeGroupDataItem* m_nodeGroupDataItem;
	PreProcessorGridAttributeCellGroupDataItem* m_cellGroupDataItem;
	PreProcessorBCGroupDataItem* m_bcGroupDataItem;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void assignActorZValues(const ZDepthRange& range) override;

	void renderGraphicsView();

	void doApplyOffset(double x, double y) override;
	virtual void setupMenu() = 0;

	class Impl;
	Impl* impl;

public:
	friend class GridPointMouseMoveCommand;
	friend class GridAttributeEditCommand;
	friend class PreProcessorGridAndGridCreatingConditionDataItem;
};

#ifdef _DEBUG
	#include "private/preprocessorgriddataitem_impl.h"
#endif // _DEBUG

#endif // PREPROCESSORGRIDDATAITEM_H
