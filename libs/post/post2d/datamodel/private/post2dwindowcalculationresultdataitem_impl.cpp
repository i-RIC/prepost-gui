#include "../post2dwindowpolydatatopdataitem.h"
#include "../post2dwindowstringresultdataitem.h"
#include "post2dwindowcalculationresultdataitem_impl.h"

Post2dWindowCalculationResultDataItem::Impl::Impl(Post2dWindowCalculationResultDataItem* item) :
	m_edgeMapper {vtkPolyDataMapper::New()},
	m_edgeActor {vtkActor::New()},
	m_gridShapeDataItem {nullptr},
	m_nodeScalarGroupTopDataItem {nullptr},
	m_cellScalarGroupTopDataItem {nullptr},
	m_iEdgeScalarGroupTopDataItem {nullptr},
	m_jEdgeScalarGroupTopDataItem {nullptr},
	m_graphGroupDataItem {nullptr},
	m_arrowGroupDataItem {nullptr},
	m_streamlineGroupDataItem {nullptr},
	m_particleGroupDataItem {nullptr},
	m_particlesDataItem {nullptr},
	m_particleGroupRootDataItem {nullptr},
	m_particleImageTopDataItem {nullptr},
	m_polyDataDataItem {nullptr},
	m_stringDataItem {nullptr},
	m_item {item}
{
	m_edgeActor->SetMapper(m_edgeMapper);
}

Post2dWindowCalculationResultDataItem::Impl::~Impl()
{
	m_edgeMapper->Delete();
	m_edgeActor->Delete();

	delete m_polyDataDataItem;
	delete m_stringDataItem;
}
