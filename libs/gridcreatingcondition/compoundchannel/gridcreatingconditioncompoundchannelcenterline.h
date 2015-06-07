#ifndef GRIDCREATINGCONDITIONCOMPOUNDCHANNELCENTERLINE_H
#define GRIDCREATINGCONDITIONCOMPOUNDCHANNELCENTERLINE_H

#include "gridcreatingconditioncompoundchannelabstractline.h"

class GridCreatingConditionCompoundChannelCenterLine : public GridCreatingConditionCompoundChannelAbstractLine
{
	Q_OBJECT

public:
	GridCreatingConditionCompoundChannelCenterLine(GridCreatingConditionCompoundChannel* parent);
};

#endif // GRIDCREATINGCONDITIONCOMPOUNDCHANNELCENTERLINE_H
