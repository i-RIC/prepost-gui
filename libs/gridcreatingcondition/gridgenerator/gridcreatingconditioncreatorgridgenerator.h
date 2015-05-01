#ifndef GRIDCREATINGCONDITIONCREATORGRIDGENERATOR_H
#define GRIDCREATINGCONDITIONCREATORGRIDGENERATOR_H

#include "gcc_gridgenerator_global.h"
#include <guicore/pre/gridcreatingcondition/gridcreatingconditioncreator.h>

class GCC_GRIDGENERATOR_EXPORT GridCreatingConditionCreatorGridGenerator : public GridCreatingConditionCreator
{
	Q_OBJECT
public:
	GridCreatingConditionCreatorGridGenerator();
	~GridCreatingConditionCreatorGridGenerator() {}

	SolverDefinitionGridType::GridType gridType() const {return SolverDefinitionGridType::gtStructured2DGrid;}
	GridCreatingCondition* create(ProjectDataItem* parent);
};

#endif // GRIDCREATINGCONDITIONCREATORGRIDGENERATOR_H
