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
	SolverDefinitionGridType::GridType gridType() const override {return SolverDefinitionGridType::gtStructured2DGrid;}
	GridCreatingCondition* create(ProjectDataItem* parent) override;
};

#endif // GRIDCREATINGCONDITIONCREATORCENTERANDWIDTH_H
