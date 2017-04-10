#ifndef GRIDCREATINGCONDITIO0NCREATORPOISSON_H
#define GRIDCREATINGCONDITIONCREATORPOISSON_H

#include "gcc_poisson_global.h"
#include <guicore/pre/gridcreatingcondition/gridcreatingconditioncreator.h>

class GCC_POISSON_EXPORT GridCreatingConditionCreatorPoisson : public GridCreatingConditionCreator
{
	Q_OBJECT

public:
	GridCreatingConditionCreatorPoisson();
	~GridCreatingConditionCreatorPoisson();

	SolverDefinitionGridType::GridType gridType() const override;
	GridCreatingCondition* create(ProjectDataItem* parent) override;
};

#endif // GRIDCREATINGCONDITIONCREATORPOISSON_H
