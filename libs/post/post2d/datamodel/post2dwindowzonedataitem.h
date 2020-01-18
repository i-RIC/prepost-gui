#ifndef POST2DWINDOWZONEDATAITEM_H
#define POST2DWINDOWZONEDATAITEM_H

#include "../post2dwindowdataitem.h"

#include <guicore/post/postzonedataitem.h>

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPolyData.h>
#include <vtkDataSetMapper.h>
#include <cgnslib.h>

class QAction;
class QSignalMapper;

class Post2dWindowCellFlagGroupDataItem;
class Post2dWindowCellScalarGroupTopDataItem;
class Post2dWindowEdgeIScalarGroupTopDataItem;
class Post2dWindowEdgeJScalarGroupTopDataItem;
class Post2dWindowGraphGroupDataItem;
class Post2dWindowGridShapeDataItem;
class Post2dWindowNodeScalarGroupTopDataItem;
class Post2dWindowNodeVectorArrowGroupDataItem;
class Post2dWindowNodeVectorStreamlineGroupDataItem;
class Post2dWindowNodeVectorParticleGroupDataItem;
class Post2dWindowParticleGroupRootDataItem;
class Post2dWindowParticlesTopDataItem;
class Post2dWindowPolyDataTopDataItem;
class PostStringResultDataItem;
class PostZoneDataContainer;

class Post2dWindowZoneDataItem : public Post2dWindowDataItem, public PostZoneDataItem
{
	Q_OBJECT

public:
	Post2dWindowZoneDataItem(const std::string& zoneName, int zoneNumber, Post2dWindowDataItem* parent);
	~Post2dWindowZoneDataItem();

	void addCustomMenuItems(QMenu* menu) override;

	// Standard mouse event handlers
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;

	void updateZDepthRangeItemCount() override;

	PostZoneDataContainer* dataContainer() override;
	vtkPolyData* filteredData() const;
	vtkPolyData* filteredData(GridLocation_t location) const;
	bool isMasked() const;
	int zoneNumber() const;
	std::string zoneName() const;
	void update(bool noparticle = false);
	Post2dWindowGridShapeDataItem* gridShapeDataItem() const;
	Post2dWindowNodeScalarGroupTopDataItem* scalarGroupTopDataItem() const;
	Post2dWindowNodeVectorArrowGroupDataItem* arrowGroupDataItem() const;
	Post2dWindowNodeVectorStreamlineGroupDataItem* streamlineDataItem() const;
	Post2dWindowNodeVectorParticleGroupDataItem* particleDataItem() const;
	Post2dWindowCellFlagGroupDataItem* cellFlagGroupDataItem() const;
	Post2dWindowCellScalarGroupTopDataItem* cellScalarGroupTopDataItem() const;
	Post2dWindowEdgeIScalarGroupTopDataItem* edgeIScalarGroupTopDataItem() const;
	Post2dWindowEdgeJScalarGroupTopDataItem* edgeJScalarGroupTopDataItem() const;
	Post2dWindowParticlesTopDataItem* particlesDataItem() const;
	Post2dWindowPolyDataTopDataItem* polyDataDataItem() const;
	Post2dWindowGraphGroupDataItem* graphGroupDataItem() const;
	PostStringResultDataItem* stringDataItem() const;

	void initCellInputAttributeBrowser();
	void clearCellInputAttributeBrowser();
	void fixCellInputAttributeBrowser(const QPoint& p, VTKGraphicsView* v);
	void updateCellInputAttributeBrowser(const QPoint& p, VTKGraphicsView* v);

	void initNodeResultAttributeBrowser();
	void clearNodeResultAttributeBrowser();
	void fixNodeResultAttributeBrowser(const QPoint& p, VTKGraphicsView* v);
	void fixEdgeIResultAttributeBrowser(const QPoint& p, VTKGraphicsView* v);
	void fixEdgeJResultAttributeBrowser(const QPoint& p, VTKGraphicsView* v);
	void updateNodeResultAttributeBrowser(const QPoint& p, VTKGraphicsView* v);
	void updateEdgeIResultAttributeBrowser(const QPoint& p, VTKGraphicsView* v);
	void updateEdgeJResultAttributeBrowser(const QPoint& p, VTKGraphicsView* v);

	void initCellResultAttributeBrowser();
	void clearCellResultAttributeBrowser();
	void fixCellResultAttributeBrowser(const QPoint& p, VTKGraphicsView* v);
	void updateCellResultAttributeBrowser(const QPoint& p, VTKGraphicsView* v);

	void initParticleResultAttributeBrowser(vtkPolyData* data);
	void clearParticleResultAttributeBrowser();
	void fixParticleResultAttributeBrowser(const QPoint& p, VTKGraphicsView* v);
	void updateParticleResultAttributeBrowser(const QPoint& p, VTKGraphicsView* v);

	void initPolyDataResultAttributeBrowser();
	void clearPolyDataResultAttributeBrowser();
	void fixPolyDataResultAttributeBrowser(const std::string& name, const QPoint& p, VTKGraphicsView* v);
	void updatePolyDataResultAttributeBrowser(const std::string& name, const QPoint& p, VTKGraphicsView* v);

	void updateRegionPolyData();

	QAction* showAttributeBrowserActionForCellInput() const;
	QAction* showAttributeBrowserActionForNodeResult() const;
	QAction* showAttributeBrowserActionForCellResult() const;
	QAction* showAttributeBrowserActionForPolyDataResult() const;

public slots:
	void showNodeAttributeBrowser();
	void showCellAttributeBrowser();
	void showPolyDataBrowser();

private:
	void assignActorZValues(const ZDepthRange& range) override;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void doViewOperationEndedGlobal(VTKGraphicsView* v) override;

	void setupActors();

	vtkIdType findVertex(const QPoint& p, VTKGraphicsView* v);
	vtkIdType findCell(const QPoint& p, VTKGraphicsView* v);
	vtkIdType findParticle(const QPoint& p, VTKGraphicsView* v);
	int findPolyDataCell(const std::string& name, const QPoint& p, VTKGraphicsView* v);
	void updateCellInputAttributeBrowser(vtkIdType cellid, VTKGraphicsView* v);
	void updateNodeResultAttributeBrowser(vtkIdType vid, double x, double y, VTKGraphicsView* v);
	void updateCellResultAttributeBrowser(vtkIdType cellid, VTKGraphicsView* v);
	void updateParticleResultAttributeBrowser(vtkIdType particleid, double x, double y, VTKGraphicsView* v);
	void updatePolyDataResultAttributeBrowser(const std::string& name, int cellid, VTKGraphicsView* v);
	void updateNodeResultAttributeBrowser(const QPoint& p, VTKGraphicsView* v, GridLocation_t gridLocation);
	void updateNodeResultAttributeBrowser(vtkIdType vid, double x, double y, VTKGraphicsView* /*v*/, GridLocation_t gridLocation);
	void fixNodeResultAttributeBrowser(const QPoint& p, VTKGraphicsView* v, GridLocation_t gridLocation);

	Post2dWindowGridShapeDataItem* m_shapeDataItem;
	Post2dWindowNodeScalarGroupTopDataItem* m_scalarGroupTopDataItem;
	Post2dWindowCellScalarGroupTopDataItem* m_cellScalarGroupTopDataItem;
	Post2dWindowEdgeIScalarGroupTopDataItem* m_edgeIScalarGroupTopDataItem;
	Post2dWindowEdgeJScalarGroupTopDataItem* m_edgeJScalarGroupTopDataItem;
	Post2dWindowNodeVectorArrowGroupDataItem* m_arrowGroupDataItem;
	Post2dWindowNodeVectorStreamlineGroupDataItem* m_streamlineGroupDataItem;
	Post2dWindowNodeVectorParticleGroupDataItem* m_particleGroupDataItem;
	Post2dWindowCellFlagGroupDataItem* m_cellFlagGroupDataItem;
	Post2dWindowParticlesTopDataItem* m_particlesDataItem;
	Post2dWindowParticleGroupRootDataItem* m_particleGroupRootDataItem;
	Post2dWindowPolyDataTopDataItem* m_polyDataDataItem;
	Post2dWindowGraphGroupDataItem* m_graphGroupDataItem;
	PostStringResultDataItem* m_stringDataItem;

	vtkSmartPointer<vtkPolyData> m_regionPolyData;
	vtkSmartPointer<vtkPolyDataMapper> m_regionMapper;
	vtkSmartPointer<vtkActor> m_regionActor;

	vtkSmartPointer<vtkPolyData> m_filteredData;
	vtkSmartPointer<vtkPolyData> m_filteredEdgeIData;
	vtkSmartPointer<vtkPolyData> m_filteredEdgeJData;

	QAction* m_showAttributeBrowserActionForCellInput;
	QAction* m_showAttributeBrowserActionForNodeResult;
	QAction* m_showAttributeBrowserActionForCellResult;
	QAction* m_showAttributeBrowserActionForPolyDataResult;

	vtkPolyData* m_currentParticlesData;

	std::string m_zoneName;
	int m_zoneNumber;
	bool m_attributeBrowserFixed;
	bool m_isMasked;
};

#endif // POST2DWINDOWZONEDATAITEM_H
