#ifndef SOLVERDEFINITIONINTEGEROPTIONCELLGRIDRELATEDCONDITION_H
#define SOLVERDEFINITIONINTEGEROPTIONCELLGRIDRELATEDCONDITION_H

#include "solverdefinitionintegercellgridrelatedcondition.h"
#include "enumloader.h"

class SolverDefinitionIntegerOptionCellGridRelatedCondition : public SolverDefinitionIntegerCellGridRelatedCondition, public IntegerEnumLoader
{
public:
	SolverDefinitionIntegerOptionCellGridRelatedCondition(QDomElement node, const SolverDefinitionTranslator& translator);
	GridRelatedConditionEditWidget* editWidget(QWidget* parent) override;
	GridRelatedConditionVariationEditWidget* variationEditWidget(QWidget* /*parent*/) override {return 0;}
	ScalarsToColorsEditWidget* createScalarsToColorsEditWidget(QWidget* parent) override;
	ScalarsToColorsContainer* createScalarsToColorsContainer(ProjectDataItem* d) override;
};

#endif // SOLVERDEFINITIONINTEGEROPTIONCELLGRIDRELATEDCONDITION_H
