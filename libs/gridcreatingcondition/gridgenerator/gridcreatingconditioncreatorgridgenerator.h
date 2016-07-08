#ifndef GRIDCREATINGCONDITIONCREATORGRIDGENERATOR_H
#define GRIDCREATINGCONDITIONCREATORGRIDGENERATOR_H

#include "gcc_gridgenerator_global.h"
#include <guicore/pre/gridcreatingcondition/gridcreatingconditioncreator.h>

class GCC_GRIDGENERATOR_EXPORT GridCreatingConditionCreatorGridGenerator : public GridCreatingConditionCreator
{
	Q_OBJECT

public:
	GridCreatingConditionCreatorGridGenerator();
	~GridCreatingConditionCreatorGridGenerator();

	SolverDefinitionGridType::GridType gridType() const override;
	GridCreatingCondition* create(ProjectDataItem* parent) override;
};

#endif // GRIDCREATINGCONDITIONCREATORGRIDGENERATOR_H
