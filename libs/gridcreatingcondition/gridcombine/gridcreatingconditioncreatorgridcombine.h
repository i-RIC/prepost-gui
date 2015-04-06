#ifndef GRIDCREATINGCONDITIONCREATORGRIDCOMBINE_H
#define GRIDCREATINGCONDITIONCREATORGRIDCOMBINE_H

#include "gcc_gridcombine_global.h"
#include <guicore/pre/gridcreatingcondition/gridcreatingconditioncreator.h>

class GCC_GRIDCOMBINE_EXPORT GridCreatingConditionCreatorGridCombine : public GridCreatingConditionCreator
{
	Q_OBJECT
public:
	GridCreatingConditionCreatorGridCombine();
	~GridCreatingConditionCreatorGridCombine(){}

	SolverDefinitionGridType::GridType gridType() const {return SolverDefinitionGridType::gtStructured2DGrid;}
	GridCreatingCondition* create(ProjectDataItem* parent);
};

#endif // GRIDCREATINGCONDITIONCREATORGRIDCOMBINE_H
