#ifndef GRIDCREATINGCONDITIONCREATORRIVERSURVEY15D_H
#define GRIDCREATINGCONDITIONCREATORRIVERSURVEY15D_H

#include "gcc_riversurvey15d_global.h"
#include <guicore/pre/gridcreatingcondition/gridcreatingconditioncreator.h>

class GCC_RIVERSURVEY15D_EXPORT GridCreatingConditionCreatorRiverSurvey15D : public GridCreatingConditionCreator
{
	Q_OBJECT

public:
	GridCreatingConditionCreatorRiverSurvey15D();
	virtual ~GridCreatingConditionCreatorRiverSurvey15D();

	SolverDefinitionGridType::GridType gridType() const override;
	GridCreatingCondition* create(ProjectDataItem* parent) override;
	bool checkAvailability(PreProcessorGridCreatingConditionDataItemInterface*) const override;
};

#endif // GRIDCREATINGCONDITIONCREATORRIVERSURVEY15D_H
