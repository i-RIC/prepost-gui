#ifndef POST3DWINDOWZONEDATAITEM_H
#define POST3DWINDOWZONEDATAITEM_H

#include "../post3dwindowdataitem.h"

#include <guicore/post/postzonedataitem.h>

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkMapper.h>
#include <vtkUnstructuredGrid.h>
#include <vtkDataSetMapper.h>

#include <string>

class QSignalMapper;

class Post3dWindowGridShapeDataItem;
class Post3dWindowCellContourGroupTopDataItem;
class Post3dWindowContourGroupTopDataItem;
class Post3dWindowNodeScalarGroupDataItem;
class Post3dWindowNodeScalarGroupTopDataItem;
class Post3dWindowArrowGroupDataItem;
class Post3dWindowNodeVectorStreamlineGroupDataItem;
class Post3dWindowNodeVectorParticleGroupDataItem;
class Post3dWindowParticleGroupRootDataItem;
class Post3dWindowParticlesTopDataItem;
class PostStringResultDataItem;
class PostZoneDataContainer;

class Post3dWindowZoneDataItem : public Post3dWindowDataItem, public PostZoneDataItem
{
	Q_OBJECT

public:
	Post3dWindowZoneDataItem(const std::string& zoneName, int zoneNumber, Post3dWindowDataItem* parent);
	~Post3dWindowZoneDataItem() override;

	// Standard mouse event handlers
	void informSelection(VTKGraphicsView*) override;
	void informDeselection(VTKGraphicsView*) override;

	PostZoneDataContainer* dataContainer() override;
	int zoneNumber() const;
	const std::string& zoneName() const;
	void update();

	Post3dWindowGridShapeDataItem* gridShapeDataItem() const;
	Post3dWindowContourGroupTopDataItem* contourGroupTopItem() const;
	Post3dWindowCellContourGroupTopDataItem* cellContourGroupTopItem() const;
	Post3dWindowNodeScalarGroupTopDataItem* scalarGroupDataItem() const;
	Post3dWindowArrowGroupDataItem* arrowGroupDataItem() const;
	Post3dWindowNodeVectorStreamlineGroupDataItem* streamlineGroupDataItem() const;
	Post3dWindowNodeVectorParticleGroupDataItem* particleGroupDataItem() const;
	Post3dWindowParticlesTopDataItem* particlesDataItem() const;
	PostStringResultDataItem* stringDataItem() const;

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

private:
	Post3dWindowGridShapeDataItem* m_shapeDataItem;
	Post3dWindowContourGroupTopDataItem* m_contourGroupTopItem;
	Post3dWindowCellContourGroupTopDataItem* m_cellContourGroupTopItem;
	Post3dWindowNodeScalarGroupTopDataItem* m_scalarGroupDataItem;
	Post3dWindowArrowGroupDataItem* m_arrowGroupDataItem;
	Post3dWindowNodeVectorStreamlineGroupDataItem* m_streamlineGroupDataItem;
	Post3dWindowNodeVectorParticleGroupDataItem* m_particleGroupDataItem;
	Post3dWindowParticlesTopDataItem* m_particlesDataItem;
	Post3dWindowParticleGroupRootDataItem* m_particleGroupRootDataItem;
	PostStringResultDataItem* m_stringDataItem;

	std::string m_zoneName;
	int m_zoneNumber;
};

#endif // POST3DWINDOWZONEDATAITEM_H
