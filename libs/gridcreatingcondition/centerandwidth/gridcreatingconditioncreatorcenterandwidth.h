#ifndef GRIDCREATINGCONDITIONCREATORCENTERANDWIDTH_H
#define GRIDCREATINGCONDITIONCREATORCENTERANDWIDTH_H

#include "gcc_centerandwidth_global.h"
#include <guicore/pre/gridcreatingcondition/gridcreatingconditioncreator.h>

class GCC_CENTERANDWIDTH_EXPORT GridCreatingConditionCreatorCenterAndWidth : public GridCreatingConditionCreator
{
	Q_OBJECT
public:
	GridCreatingConditionCreatorCenterAndWidth();
	~GridCreatingConditionCreatorCenterAndWidth() {}
	// temporary
	SolverDefinitionGridType::GridType gridType() const {return SolverDefinitionGridType::gtStructured2DGrid;}
	GridCreatingCondition* create(ProjectDataItem* parent);
};

#endif // GRIDCREATINGCONDITIONCREATORCENTERANDWIDTH_H
