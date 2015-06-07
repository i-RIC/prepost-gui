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
	void informSelection(VTKGraphicsView*) override;
	void informDeselection(VTKGraphicsView*) override;

	void informgridRelatedConditionChange(const QString& name);

	PostZoneDataContainer* dataContainer();
	int zoneNumber() const {return m_zoneNumber;}
	QString zoneName() const {return m_zoneName;}
	void update();
	Post3dWindowGridShapeDataItem* gridShapeDataItem() const {return m_shapeDataItem;}
	Post3dWindowContourGroupDataItem* contourGroupItem() const {return m_contourGroupItem;}
	Post3dWindowNodeScalarGroupDataItem* scalarGroupDataItem() const {return m_scalarGroupDataItem;}
	Post3dWindowArrowGroupDataItem* arrowGroupDataItem() const {return m_arrowGroupDataItem;}
	Post3dWindowNodeVectorStreamlineGroupDataItem* streamlineGroupDataItem() const {return m_streamlineGroupDataItem;}
	Post3dWindowNodeVectorParticleGroupDataItem* particleGroupDataItem() const {return m_particleGroupDataItem;}
	Post3dWindowParticlesTopDataItem* particlesDataItem() const {return m_particlesDataItem;}

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

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
