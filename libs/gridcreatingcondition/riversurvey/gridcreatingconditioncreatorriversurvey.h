#ifndef GRIDCREATINGCONDITIONCREATORRIVERSURVEY_H
#define GRIDCREATINGCONDITIONCREATORRIVERSURVEY_H

#include "gcc_riversurvey_global.h"
#include <guicore/pre/gridcreatingcondition/gridcreatingconditioncreator.h>

class GCC_RIVERSURVEY_EXPORT GridCreatingConditionCreatorRiverSurvey : public GridCreatingConditionCreator
{
	Q_OBJECT

public:
	GridCreatingConditionCreatorRiverSurvey();
	~GridCreatingConditionCreatorRiverSurvey() {}
	SolverDefinitionGridType::GridType gridType() const override {return SolverDefinitionGridType::gtStructured2DGrid;}
	GridCreatingCondition* create(ProjectDataItem* parent) override;
	bool checkAvailability(PreProcessorGridCreatingConditionDataItemInterface*) const override;
};

#endif // GRIDCREATINGCONDITIONCREATORCENTERANDWIDTH_H
