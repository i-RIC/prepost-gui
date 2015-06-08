#include "gridcreatingconditioncompoundchannelgridregionpolygon.h"

#include <vtkProperty.h>

GridCreatingConditionCompoundChannelGridRegionPolygon::GridCreatingConditionCompoundChannelGridRegionPolygon(GridCreatingConditionCompoundChannel* parent)
	: GridCreatingConditionCompoundChannelAbstractPolygon(parent)
{
	m_paintActor->GetProperty()->SetColor(0, 0, 0);
	m_paintActor->GetProperty()->SetOpacity(0.1);
	m_edgeActor->GetProperty()->SetColor(0, 0, 0);
	m_vertexActor->GetProperty()->SetColor(0, 0, 0);
}
