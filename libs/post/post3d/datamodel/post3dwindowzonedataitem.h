#ifndef POST3DWINDOWZONEDATAITEM_H
#define POST3DWINDOWZONEDATAITEM_H

#include "../post3dwindowdataitem.h"
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkMapper.h>
#include <vtkUnstructuredGrid.h>
#include <vtkDataSetMapper.h>

class QSignalMapper;

class Post3dWindowGridShapeDataItem;
class Post3dWindowContourGroupDataItem;
class Post3dWindowNodeScalarGroupDataItem;
class Post3dWindowArrowGroupDataItem;
class Post3dWindowNodeVectorStreamlineGroupDataItem;
class Post3dWindowNodeVectorParticleGroupDataItem;
class Post3dWindowParticlesTopDataItem;
class PostZoneDataContainer;

class Post3dWindowZoneDataItem : public Post3dWindowDataItem
{
	Q_OBJECT
public:
	/// Constructor
	Post3dWindowZoneDataItem(QString zoneName, int zoneNumber, Post3dWindowDataItem* parent);

	// Standard mouse event handlers
	void informSelection(VTKGraphicsView *);
	void informDeselection(VTKGraphicsView *);

	void informgridRelatedConditionChange(const QString& name);

	PostZoneDataContainer* dataContainer();
	int zoneNumber(){return m_zoneNumber;}
	QString zoneName(){return m_zoneName;}
	void update();
	Post3dWindowGridShapeDataItem* gridShapeDataItem(){return m_shapeDataItem;}
	Post3dWindowContourGroupDataItem* contourGroupItem(){return m_contourGroupItem;}
	Post3dWindowNodeScalarGroupDataItem* scalarGroupDataItem(){return m_scalarGroupDataItem;}
	Post3dWindowArrowGroupDataItem* arrowGroupDataItem(){return m_arrowGroupDataItem;}
	Post3dWindowNodeVectorStreamlineGroupDataItem* streamlineGroupDataItem(){return m_streamlineGroupDataItem;}
	Post3dWindowNodeVectorParticleGroupDataItem* particleGroupDataItem(){return m_particleGroupDataItem;}
	Post3dWindowParticlesTopDataItem* particlesDataItem(){return m_particlesDataItem;}

protected:
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);

private:
	Post3dWindowGridShapeDataItem* m_shapeDataItem;
	Post3dWindowContourGroupDataItem* m_contourGroupItem;
	Post3dWindowNodeScalarGroupDataItem* m_scalarGroupDataItem;
	Post3dWindowArrowGroupDataItem* m_arrowGroupDataItem;
	Post3dWindowNodeVectorStreamlineGroupDataItem* m_streamlineGroupDataItem;
	Post3dWindowNodeVectorParticleGroupDataItem* m_particleGroupDataItem;
	Post3dWindowParticlesTopDataItem* m_particlesDataItem;

	QString m_zoneName;
	int m_zoneNumber;
};

#endif // POST3DWINDOWZONEDATAITEM_H
