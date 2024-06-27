#include "post2dwindowinputgriddataitem_impl.h"

#include <vtkActor.h>
#include <vtkPolyDataMapper.h>

Post2dWindowInputGridDataItem::Impl::Impl() :
	m_gridShapeDataItem {nullptr},
	m_nodeGroupDataItem {nullptr},
	m_cellGroupDataItem {nullptr},
	m_iEdgeGroupDataItem {nullptr},
	m_jEdgeGroupDataItem {nullptr}
{}

Post2dWindowInputGridDataItem::Impl::~Impl()
{}
