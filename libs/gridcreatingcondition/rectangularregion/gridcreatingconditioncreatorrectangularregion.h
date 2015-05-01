#ifndef GRIDCREATINGCONDITIONCREATORRECTANGULARREGION_H
#define GRIDCREATINGCONDITIONCREATORRECTANGULARREGION_H

#include "gcc_rectangularregion_global.h"
#include <guicore/pre/gridcreatingcondition/gridcreatingconditioncreator.h>

class GCC_RECTANGULARREGION_EXPORT GridCreatingConditionCreatorRectangularRegion : public GridCreatingConditionCreator
{
	Q_OBJECT
public:
	GridCreatingConditionCreatorRectangularRegion();
	~GridCreatingConditionCreatorRectangularRegion() {}

	SolverDefinitionGridType::GridType gridType() const {return SolverDefinitionGridType::gtStructured2DGrid;}
	GridCreatingCondition* create(ProjectDataItem* parent);
};

#endif // GRIDCREATINGCONDITIONCREATORRECTANGULARREGION_H
