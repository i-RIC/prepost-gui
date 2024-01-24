#ifndef POST2DWINDOWCALCULATIONRESULTDATAITEM_H
#define POST2DWINDOWCALCULATIONRESULTDATAITEM_H

#include "post2dwindowgriddataitemi.h"

#include <memory>

class Post2dWindowGridShapeDataItem;
class Post2dWindowCellScalarGroupTopDataItem;
class Post2dWindowGraphGroupDataItem;
class Post2dWindowIEdgeScalarGroupTopDataItem;
class Post2dWindowJEdgeScalarGroupTopDataItem;
class Post2dWindowNodeScalarGroupTopDataItem;
class Post2dWindowNodeVectorArrowGroupTopDataItem;
class Post2dWindowNodeVectorParticleGroupDataItem;
class Post2dWindowNodeVectorStreamlineGroupDataItem;
class Post2dWindowParticleGroupRootDataItem;
class Post2dWindowParticlesTopDataItem;
class Post2dWindowPolyDataTopDataItem;
class Post2dWindowStringResultDataItem;
class Post2dWindowZoneDataItem;

class Post2dWindowCalculationResultDataItem : public Post2dWindowGridDataItemI
{
	Q_OBJECT

public:
	Post2dWindowCalculationResultDataItem(Post2dWindowDataItem* parent);
	~Post2dWindowCalculationResultDataItem();

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void updateZDepthRangeItemCount() override;

	void update(bool noParticle);
	v4Grid2d* grid() const override;

	Post2dWindowZoneDataItem* zoneDataItem() const;
	Post2dWindowGridShapeDataItem* gridShapeDataItem() const;
	Post2dWindowNodeScalarGroupTopDataItem* nodeScalarGroupTopDataItem() const;
	Post2dWindowCellScalarGroupTopDataItem* cellScalarGroupTopDataItem() const;
	Post2dWindowIEdgeScalarGroupTopDataItem* iEdgeScalarGroupTopDataItem() const;
	Post2dWindowJEdgeScalarGroupTopDataItem* jEdgeScalarGroupTopDataItem() const;
	Post2dWindowGraphGroupDataItem* graphGroupDataItem() const;
	Post2dWindowNodeVectorArrowGroupTopDataItem* arrowGroupDataItem() const;
	Post2dWindowNodeVectorStreamlineGroupDataItem* streamlineDataItem() const;
	Post2dWindowNodeVectorParticleGroupDataItem* particleGroupDataItem() const;
	Post2dWindowParticlesTopDataItem* particlesDataItem() const;
	Post2dWindowParticleGroupRootDataItem* particleGroupRootDataItem() const;
	Post2dWindowPolyDataTopDataItem* polyDataDataItem() const;
	Post2dWindowStringResultDataItem* stringDataItem() const;

private:
	void assignActorZValues(const ZDepthRange& range) override;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	class Impl;
	std::unique_ptr<Impl> impl;
};

#endif // POST2DWINDOWCALCULATIONRESULTDATAITEM_H
