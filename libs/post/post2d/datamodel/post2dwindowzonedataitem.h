#ifndef POST2DWINDOWZONEDATAITEM_H
#define POST2DWINDOWZONEDATAITEM_H

#include "../post2dwindowdataitem.h"
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPolyData.h>
#include <vtkDataSetMapper.h>

class QAction;
class QSignalMapper;

class Post2dWindowGraphGroupDataItem;
class Post2dWindowGridShapeDataItem;
class Post2dWindowNodeScalarGroupTopDataItem;
class Post2dWindowCellScalarGroupTopDataItem;
class Post2dWindowNodeVectorArrowGroupDataItem;
class Post2dWindowNodeVectorStreamlineGroupDataItem;
class Post2dWindowNodeVectorParticleGroupDataItem;
class Post2dWindowCellFlagGroupDataItem;
class Post2dWindowParticlesTopDataItem;
class PostZoneDataContainer;

class Post2dWindowZoneDataItem : public Post2dWindowDataItem
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

	PostZoneDataContainer* dataContainer();
	vtkPolyData* filteredData() const;
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
	Post2dWindowParticlesTopDataItem* particlesDataItem() const;
	Post2dWindowGraphGroupDataItem* graphGroupDataItem() const;

	void initCellInputAttributeBrowser();
	void clearCellInputAttributeBrowser();
	void fixCellInputAttributeBrowser(const QPoint& p, VTKGraphicsView* v);
	void updateCellInputAttributeBrowser(const QPoint& p, VTKGraphicsView* v);

	void initNodeResultAttributeBrowser();
	void clearNodeResultAttributeBrowser();
	void fixNodeResultAttributeBrowser(const QPoint& p, VTKGraphicsView* v);
	void updateNodeResultAttributeBrowser(const QPoint& p, VTKGraphicsView* v);

	void initCellResultAttributeBrowser();
	void clearCellResultAttributeBrowser();
	void fixCellResultAttributeBrowser(const QPoint& p, VTKGraphicsView* v);
	void updateCellResultAttributeBrowser(const QPoint& p, VTKGraphicsView* v);

	void initParticleResultAttributeBrowser();
	void clearParticleResultAttributeBrowser();
	void fixParticleResultAttributeBrowser(const QPoint& p, VTKGraphicsView* v);
	void updateParticleResultAttributeBrowser(const QPoint& p, VTKGraphicsView* v);

	void updateRegionPolyData();

	QAction* showAttributeBrowserActionForCellInput() const;
	QAction* showAttributeBrowserActionForNodeResult() const;
	QAction* showAttributeBrowserActionForCellResult() const;
	QAction* showAttributeBrowserActionForParticleResult() const;

public slots:
	void showNodeAttributeBrowser();
	void showCellAttributeBrowser();
	void showParticleBrowser();

private:
	void assignActorZValues(const ZDepthRange& range) override;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void doViewOperationEndedGlobal(VTKGraphicsView* v) override;

	void setupActors();

	vtkIdType findVertex(const QPoint& p, VTKGraphicsView* v);
	vtkIdType findCell(const QPoint& p, VTKGraphicsView* v);
	void updateCellInputAttributeBrowser(vtkIdType cellid, VTKGraphicsView* v);
	void updateNodeResultAttributeBrowser(vtkIdType vid, double x, double y, VTKGraphicsView* v);
	void updateCellResultAttributeBrowser(vtkIdType cellid, VTKGraphicsView* v);
	vtkIdType findParticle(const QPoint& p, VTKGraphicsView* v);
	void updateParticleResultAttributeBrowser(vtkIdType particleid, double x, double y, VTKGraphicsView* v);

	Post2dWindowGridShapeDataItem* m_shapeDataItem;
	Post2dWindowNodeScalarGroupTopDataItem* m_scalarGroupTopDataItem;
	Post2dWindowCellScalarGroupTopDataItem* m_cellScalarGroupTopDataItem;
	Post2dWindowNodeVectorArrowGroupDataItem* m_arrowGroupDataItem;
	Post2dWindowNodeVectorStreamlineGroupDataItem* m_streamlineGroupDataItem;
	Post2dWindowNodeVectorParticleGroupDataItem* m_particleGroupDataItem;
	Post2dWindowCellFlagGroupDataItem* m_cellFlagGroupDataItem;
	Post2dWindowParticlesTopDataItem* m_particlesDataItem;
	Post2dWindowGraphGroupDataItem* m_graphGroupDataItem;

	vtkSmartPointer<vtkPolyData> m_regionPolyData;
	vtkSmartPointer<vtkPolyDataMapper> m_regionMapper;
	vtkSmartPointer<vtkActor> m_regionActor;

	vtkSmartPointer<vtkPolyData> m_filteredData;

	QAction* m_showAttributeBrowserActionForCellInput;
	QAction* m_showAttributeBrowserActionForNodeResult;
	QAction* m_showAttributeBrowserActionForCellResult;
	QAction* m_showAttributeBrowserActionForParticleResult;

	std::string m_zoneName;
	int m_zoneNumber;
	bool m_attributeBrowserFixed;
	bool m_isMasked;
};

#endif // POST2DWINDOWZONEDATAITEM_H
