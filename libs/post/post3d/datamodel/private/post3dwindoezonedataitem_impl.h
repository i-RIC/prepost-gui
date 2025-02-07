#ifndef POST3DWINDOEZONEDATAITEM_IMPL_H
#define POST3DWINDOEZONEDATAITEM_IMPL_H

#include "../post3dwindowzonedataitem.h"

class Post3dWindowZoneDataItem::Impl
{
public:
	Impl(const std::string& zoneName);
	~Impl();

	Post3dWindowGridShapeDataItem* m_shapeDataItem;
	Post3dWindowContourGroupTopDataItem* m_contourGroupTopItem;
	Post3dWindowCellContourGroupTopDataItem* m_cellContourGroupTopItem;
	Post3dWindowIFaceContourGroupTopDataItem* m_iFaceContourGroupTopItem;
	Post3dWindowJFaceContourGroupTopDataItem* m_jFaceContourGroupTopItem;
	Post3dWindowKFaceContourGroupTopDataItem* m_kFaceContourGroupTopItem;
	Post3dWindowNodeScalarGroupTopDataItem* m_scalarGroupDataItem;
	Post3dWindowNodeVectorArrowTopDataItem* m_arrowTopDataItem;
	Post3dWindowNodeVectorStreamlineGroupDataItem* m_streamlineGroupDataItem;
	Post3dWindowNodeVectorParticleGroupDataItem* m_particleGroupDataItem;
	Post3dWindowParticlesTopDataItem* m_particlesDataItem;
	Post3dWindowParticleGroupRootDataItem* m_particleGroupRootDataItem;
	Post3dWindowStringResultDataItem* m_stringDataItem;

	vtkPolyDataMapper* m_edgeMapper;
	vtkActor* m_edgeActor;

	std::string m_zoneName;
};

#endif // POST3DWINDOEZONEDATAITEM_IMPL_H
