#ifndef SOLVERDEFINITIONREALOPTIONCELLGRIDRELATEDCONDITION_H
#define SOLVERDEFINITIONREALOPTIONCELLGRIDRELATEDCONDITION_H

#include "solverdefinitionrealcellgridrelatedcondition.h"
#include "enumloader.h"

class SolverDefinitionRealOptionCellGridRelatedCondition : public SolverDefinitionRealCellGridRelatedCondition, public RealEnumLoader
{
public:
	SolverDefinitionRealOptionCellGridRelatedCondition(QDomElement node, const SolverDefinitionTranslator& translator);
	GridRelatedConditionEditWidget* editWidget(QWidget* parent);
	GridRelatedConditionVariationEditWidget* variationEditWidget(QWidget* /*parent*/) {return 0;}
	ScalarsToColorsEditWidget* createScalarsToColorsEditWidget(QWidget* parent);
	ScalarsToColorsContainer* createScalarsToColorsContainer(ProjectDataItem* d);
};

#endif // SOLVERDEFINITIONREALOPTIONCELLGRIDRELATEDCONDITION_H
