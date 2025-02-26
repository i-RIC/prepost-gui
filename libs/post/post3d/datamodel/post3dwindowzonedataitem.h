#ifndef POST3DWINDOWZONEDATAITEM_H
#define POST3DWINDOWZONEDATAITEM_H

#include "../post3dwindowdataitem.h"

#include <guicore/post/postzonedataitem.h>

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkMapper.h>
#include <vtkUnstructuredGrid.h>
#include <vtkDataSetMapper.h>

#include <memory>
#include <string>

class QSignalMapper;

class Post3dWindowGridShapeDataItem;
class Post3dWindowGridTypeDataItem;




class Post3dWindowCellContourGroupTopDataItem;
class Post3dWindowContourGroupTopDataItem;
class Post3dWindowIFaceContourGroupTopDataItem;
class Post3dWindowJFaceContourGroupTopDataItem;
class Post3dWindowKFaceContourGroupTopDataItem;
class Post3dWindowNodeScalarGroupDataItem;
class Post3dWindowNodeScalarGroupTopDataItem;
class Post3dWindowNodeVectorArrowTopDataItem;
class Post3dWindowNodeVectorStreamlineGroupDataItem;
class Post3dWindowNodeVectorParticleGroupDataItem;
class Post3dWindowParticleGroupRootDataItem;
class Post3dWindowParticlesTopDataItem;
class Post3dWindowStringResultDataItem;
class v4PostZoneDataContainer;

class Post3dWindowZoneDataItem : public Post3dWindowDataItem, public PostZoneDataItem
{
	Q_OBJECT

public:
	Post3dWindowZoneDataItem(const std::string& zoneName, Post3dWindowDataItem* parent);
	~Post3dWindowZoneDataItem() override;

	// Standard mouse event handlers
	void informSelection(VTKGraphicsView*) override;
	void informDeselection(VTKGraphicsView*) override;

	v4PostZoneDataContainer* v4DataContainer() override;
	const std::string& zoneName() const;
	void setEdgeFocus(vtkIdType i, vtkIdType j, vtkIdType k);
	void clearEdgeFocus();
	void update();

	Post3dWindowGridTypeDataItem* gridTypeDataItem() const;
	Post3dWindowGridShapeDataItem* gridShapeDataItem() const;
	Post3dWindowContourGroupTopDataItem* contourGroupTopItem() const;
	Post3dWindowCellContourGroupTopDataItem* cellContourGroupTopItem() const;
	Post3dWindowIFaceContourGroupTopDataItem* iFaceContourGroupTopItem() const;
	Post3dWindowJFaceContourGroupTopDataItem* jFaceContourGroupTopItem() const;
	Post3dWindowKFaceContourGroupTopDataItem* kFaceContourGroupTopItem() const;
	Post3dWindowNodeScalarGroupTopDataItem* scalarGroupDataItem() const;
	Post3dWindowNodeVectorArrowTopDataItem* arrowTopDataItem() const;
	Post3dWindowNodeVectorStreamlineGroupDataItem* streamlineGroupDataItem() const;
	Post3dWindowNodeVectorParticleGroupDataItem* particleGroupDataItem() const;
	Post3dWindowParticlesTopDataItem* particlesDataItem() const;
	Post3dWindowStringResultDataItem* stringDataItem() const;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	class Impl;
	std::unique_ptr<Impl> impl;
};

#endif // POST3DWINDOWZONEDATAITEM_H
