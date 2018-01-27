#ifndef POST2DWINDOWZONEDATAITEM_H
#define POST2DWINDOWZONEDATAITEM_H

#include "../post2dbirdeyewindowdataitem.h"
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkMapper.h>
#include <vtkUnstructuredGrid.h>
#include <vtkDataSetMapper.h>

class QSignalMapper;

class Post2dBirdEyeWindowGridShapeDataItem;
class Post2dBirdEyeWindowContourDataItem;
class Post2dBirdEyeWindowCellScalarGroupDataItem;
class Post2dBirdEyeWindowCellScalarGroupTopDataItem;
class Post2dBirdEyeWindowNodeScalarGroupDataItem;
class Post2dBirdEyeWindowNodeScalarGroupTopDataItem;
class Post2dBirdEyeWindowNodeVectorArrowGroupDataItem;
class Post2dBirdEyeWindowNodeVectorStreamlineGroupDataItem;
class Post2dBirdEyeWindowNodeVectorParticleGroupDataItem;
class Post2dBirdEyeWindowCellFlagGroupDataItem;
class PostZoneDataContainer;

class Post2dBirdEyeWindowZoneDataItem : public Post2dBirdEyeWindowDataItem
{
	Q_OBJECT

public:
	/// Constructor
	Post2dBirdEyeWindowZoneDataItem(const std::string& zoneName, int zoneNumber, GraphicsWindowDataItem* parent);
	void addCustomMenuItems(QMenu* menu) override;

	// Standard mouse event handlers
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;

	void updateZDepthRangeItemCount() override;

	PostZoneDataContainer* dataContainer();
	int zoneNumber() const {return m_zoneNumber;}
	std::string zoneName() const {return m_zoneName;}
	void update();
	Post2dBirdEyeWindowGridShapeDataItem* gridShapeDataItem() const {return m_shapeDataItem;}
	Post2dBirdEyeWindowNodeScalarGroupTopDataItem* scalarGroupTopDataItem() const {return m_scalarGroupTopDataItem;}
	Post2dBirdEyeWindowCellScalarGroupTopDataItem* cellScalarGroupTopDataItem() const {return m_cellScalarGroupTopDataItem;}
	Post2dBirdEyeWindowNodeVectorArrowGroupDataItem* arrowGroupDataItem() const {return m_arrowGroupDataItem;}
	Post2dBirdEyeWindowNodeVectorStreamlineGroupDataItem* streamlineDataItem() const {return m_streamlineGroupDataItem;}
	Post2dBirdEyeWindowNodeVectorParticleGroupDataItem* particleDataItem() const {return m_particleGroupDataItem;}
	Post2dBirdEyeWindowCellFlagGroupDataItem* cellFlagGroupDataItem() const {return m_cellFlagGroupDataItem;}

protected:
	void assignActorZValues(const ZDepthRange& range) override;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	Post2dBirdEyeWindowGridShapeDataItem* m_shapeDataItem;
	Post2dBirdEyeWindowNodeScalarGroupTopDataItem* m_scalarGroupTopDataItem;
	Post2dBirdEyeWindowCellScalarGroupTopDataItem* m_cellScalarGroupTopDataItem;
	Post2dBirdEyeWindowNodeVectorArrowGroupDataItem* m_arrowGroupDataItem;
	Post2dBirdEyeWindowNodeVectorStreamlineGroupDataItem* m_streamlineGroupDataItem;
	Post2dBirdEyeWindowNodeVectorParticleGroupDataItem* m_particleGroupDataItem;
	Post2dBirdEyeWindowCellFlagGroupDataItem* m_cellFlagGroupDataItem;

private:
	std::string m_zoneName;
	int m_zoneNumber;
};

#endif // POST2DWINDOWZONEDATAITEM_H
