#ifndef GRIDCREATINGCONDITIONCREATORLAPLACE_H
#define GRIDCREATINGCONDITIONCREATORLAPLACE_H

#include "gcc_laplace_global.h"
#include <guicore/pre/gridcreatingcondition/gridcreatingconditioncreator.h>

class GCC_LAPLACE_EXPORT GridCreatingConditionCreatorLaplace : public GridCreatingConditionCreator
{
	Q_OBJECT

public:
	GridCreatingConditionCreatorLaplace();
	~GridCreatingConditionCreatorLaplace();

	SolverDefinitionGridType::GridType gridType() const override;
	GridCreatingCondition* create(ProjectDataItem* parent) override;
};

#endif // GRIDCREATINGCONDITIONCREATORLAPLACE_H
