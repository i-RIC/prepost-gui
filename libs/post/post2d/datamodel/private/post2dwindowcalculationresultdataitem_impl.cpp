#include "../post2dwindowpolydatatopdataitem.h"
#include "post2dwindowcalculationresultdataitem_impl.h"

#include <postbase/string/poststringresultdataitem.h>

Post2dWindowCalculationResultDataItem::Impl::Impl(Post2dWindowCalculationResultDataItem* item) :
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
	m_polyDataDataItem {nullptr},
	m_stringDataItem {nullptr},
	m_item {item}
{}

Post2dWindowCalculationResultDataItem::Impl::~Impl()
{
	delete m_polyDataDataItem;
	delete m_stringDataItem;
}
