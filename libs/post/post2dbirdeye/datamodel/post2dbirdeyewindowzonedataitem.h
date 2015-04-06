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
class Post2dBirdEyeWindowNodeScalarGroupDataItem;
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
	Post2dBirdEyeWindowZoneDataItem(QString zoneName, int zoneNumber, GraphicsWindowDataItem* parent);
	void addCustomMenuItems(QMenu* menu);

	// Standard mouse event handlers
	void informSelection(VTKGraphicsView *v);
	void informDeselection(VTKGraphicsView *v);

	void informgridRelatedConditionChange(const QString& name);
	void updateZDepthRangeItemCount();

	PostZoneDataContainer* dataContainer();
	int zoneNumber(){return m_zoneNumber;}
	QString zoneName(){return m_zoneName;}
	void update();
	Post2dBirdEyeWindowGridShapeDataItem* gridShapeDataItem(){return m_shapeDataItem;}
	Post2dBirdEyeWindowNodeScalarGroupDataItem* scalarGroupDataItem(){return m_scalarGroupDataItem;}
	Post2dBirdEyeWindowNodeVectorArrowGroupDataItem* arrowGroupDataItem(){return m_arrowGroupDataItem;}
	Post2dBirdEyeWindowNodeVectorStreamlineGroupDataItem* streamlineDataItem(){return m_streamlineGroupDataItem;}
	Post2dBirdEyeWindowNodeVectorParticleGroupDataItem* particleDataItem(){return m_particleGroupDataItem;}
	Post2dBirdEyeWindowCellFlagGroupDataItem* cellFlagGroupDataItem(){return m_cellFlagGroupDataItem;}
protected:
	void assignActionZValues(const ZDepthRange& range);
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);

	Post2dBirdEyeWindowGridShapeDataItem* m_shapeDataItem;
	Post2dBirdEyeWindowNodeScalarGroupDataItem* m_scalarGroupDataItem;
	Post2dBirdEyeWindowNodeVectorArrowGroupDataItem* m_arrowGroupDataItem;
	Post2dBirdEyeWindowNodeVectorStreamlineGroupDataItem* m_streamlineGroupDataItem;
	Post2dBirdEyeWindowNodeVectorParticleGroupDataItem* m_particleGroupDataItem;
	Post2dBirdEyeWindowCellFlagGroupDataItem* m_cellFlagGroupDataItem;
private:
	QString m_zoneName;
	int m_zoneNumber;
};

#endif // POST2DWINDOWZONEDATAITEM_H
