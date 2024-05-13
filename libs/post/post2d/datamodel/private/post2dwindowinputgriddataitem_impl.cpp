#include "post2dwindowinputgriddataitem_impl.h"

#include <vtkActor.h>
#include <vtkPolyDataMapper.h>

Post2dWindowInputGridDataItem::Impl::Impl() :
	m_edgeMapper {vtkPolyDataMapper::New()},
	m_edgeActor {vtkActor::New()},
	m_gridShapeDataItem {nullptr},
	m_nodeGroupDataItem {nullptr},
	m_cellGroupDataItem {nullptr},
	m_iEdgeGroupDataItem {nullptr},
	m_jEdgeGroupDataItem {nullptr}
{
	m_edgeActor->SetMapper(m_edgeMapper);
}

Post2dWindowInputGridDataItem::Impl::~Impl()
{
	m_edgeMapper->Delete();
	m_edgeActor->Delete();
}
