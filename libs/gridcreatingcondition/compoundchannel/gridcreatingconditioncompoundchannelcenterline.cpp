#include "gridcreatingconditioncompoundchannelcenterline.h"

#include <vtkProperty.h>

GridCreatingConditionCompoundChannelCenterLine::GridCreatingConditionCompoundChannelCenterLine(GridCreatingConditionCompoundChannel* parent)
	: GridCreatingConditionCompoundChannelAbstractLine(parent)
{
	m_edgeActor->GetProperty()->SetColor(0, 0, 0);
	m_vertexActor->GetProperty()->SetColor(0, 0, 0);
}
