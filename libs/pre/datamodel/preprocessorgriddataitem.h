#ifndef PREPROCESSORGRIDDATAITEM_H
#define PREPROCESSORGRIDDATAITEM_H

#include "../subwindow/gridcrosssectionwindow/preprocessorgridcrosssectioninternalwindow.h"
#include "../subwindow/gridcrosssectionwindow/preprocessorgridcrosssectionwindow.h"

#include <guicore/pre/base/preprocessorgriddataitemi.h>
#include <misc/edge.h>

#include <QVector>

#include <unordered_set>
#include <vector>

class v4InputGrid;
class QSignalMapper;

class GridImporterI;
class PreProcessorBCGroupDataItem;
class PreProcessorGridAndGridCreatingConditionDataItem;
class PreProcessorGridAttributeCellDataItem;
class PreProcessorGridAttributeCellGroupDataItem;
class PreProcessorGridAttributeIEdgeGroupDataItem;
class PreProcessorGridAttributeIEdgeDataItem;
class PreProcessorGridAttributeJEdgeGroupDataItem;
class PreProcessorGridAttributeJEdgeDataItem;
class PreProcessorGridAttributeNodeDataItem;
class PreProcessorGridAttributeNodeGroupDataItem;
class PreProcessorGridShapeDataItem;
class PreProcessorGridTypeDataItem;
class VTK2DGraphicsView;

class GridPointMouseMoveCommand;
class GridAttributeEditCommand;

class vtkPolyData;
class QAction;

namespace iRICLib {
	class H5CgnsBase;
	class H5CgnsZone;
} // namespace iRICLib

class PreProcessorGridDataItem : public PreProcessorGridDataItemI
{
	Q_OBJECT

public:
	class SelectedAbstractCellsController;
	class SelectedDataController;
	class SelectedDataWithIdController;
	class SelectedNodesController;
	class SelectedCellsController;
	class SelectedIEdgesController;
	class SelectedJEdgesController;
	class SelectedEdgesController;

	PreProcessorGridDataItem(PreProcessorDataItem* parent);
	~PreProcessorGridDataItem();

	v4InputGrid* grid() const override;
	bool setGrid(v4InputGrid* newGrid) override;

	int loadFromCgnsFile() override;
	int loadFromCgnsFile(const iRICLib::H5CgnsZone& zone);
	int saveToCgnsFile() override;
	int saveToCgnsFile(iRICLib::H5CgnsBase* base, const std::string& zoneName);
	void closeCgnsFile() override;
	bool importFromImporter(v4InputGrid* grid, GridImporterI* importer, const QString& filename, const QString& selectedFilter);

	void updateActionStatus() override;

	void addCustomMenuItems(QMenu* menu) override;

	// Standard mouse event handlers
	void mouseDoubleClickEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;

	SelectedNodesController* selectedNodesController() const;
	SelectedCellsController* selectedCellsController() const;
	SelectedIEdgesController* selectedIEdgesController() const;
	SelectedJEdgesController* selectedJEdgesController() const;
	SelectedEdgesController* selectedEdgesController() const;

	// Mouse event handlers for grid edges selecting

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;

	void updateZDepthRangeItemCount() override;
	void informGridAttributeChangeAll();
	void informGridAttributeChange(const std::string& name) override;
	void silentDeleteGrid() override;

	PreProcessorGridTypeDataItem* gridTypeDataItem() const;
	PreProcessorGridShapeDataItem* shapeDataItem() const;
	PreProcessorGridAttributeNodeGroupDataItem* nodeGroupDataItem() const;
	PreProcessorGridAttributeCellGroupDataItem* cellGroupDataItem() const;
	PreProcessorGridAttributeIEdgeGroupDataItem* iEdgeGroupDataItem() const;
	PreProcessorGridAttributeJEdgeGroupDataItem* jEdgeGroupDataItem() const;
	PreProcessorBCGroupDataItem* bcGroupDataItem() const;
	bool colorBarShouldBeVisible(const std::string& name) const;

	std::vector<vtkIdType> getCellsFromVertices(const std::unordered_set<vtkIdType>& vertices) const;
	std::vector<Edge> getEdgesFromVertices(const std::unordered_set<vtkIdType>& vertices) const;
	std::vector<vtkIdType> getIEdgesFromVertices(const std::unordered_set<vtkIdType>& vertices) const;
	std::vector<vtkIdType> getJEdgesFromVertices(const std::unordered_set<vtkIdType>& vertices) const;

	void setBCGroupDataItem(PreProcessorBCGroupDataItem* item) override;
	void unsetBCGroupDataItem() override;
	void applyColorMapSetting(const std::string& name) override;
	void openCrossSectionWindow(PreProcessorGridCrosssectionInternalWindow::Direction dir, int index);
	void addCrossSectionWindow(PreProcessorGridCrosssectionWindow* w);
	void removeCrossSectionWindow(PreProcessorGridCrosssectionWindow* w);

	bool isImportAvailable();
	bool isExportAvailable();

	bool gridIsDeleted() const override;

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
	QAction* birdEyeWindowAction() const;
	QMenu* generateAttMenu() const;
	QMenu* menu() const;

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
	void updateVtkObjectsForDrawing();

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
	void setDimensionsToAttributes();
	void closeBirdEyeWindow();
	void closeCrosssectionWindows();

protected:
	PreProcessorGridShapeDataItem* m_shapeDataItem;
	PreProcessorGridAttributeNodeGroupDataItem* m_nodeGroupDataItem;
	PreProcessorGridAttributeCellGroupDataItem* m_cellGroupDataItem;
	PreProcessorGridAttributeIEdgeGroupDataItem* m_iEdgeGroupDataItem;
	PreProcessorGridAttributeJEdgeGroupDataItem* m_jEdgeGroupDataItem;
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

	class CgnsExporter;
	class CgnsImporter;
	class ProjectImporter;

public:
	friend class GridPointMouseMoveCommand;
	friend class GridAttributeEditCommand;
	friend class PreProcessorGridAndGridCreatingConditionDataItem;
};

#ifdef _DEBUG
	#include "private/preprocessorgriddataitem_impl.h"
#endif // _DEBUG

#endif // PREPROCESSORGRIDDATAITEM_H
