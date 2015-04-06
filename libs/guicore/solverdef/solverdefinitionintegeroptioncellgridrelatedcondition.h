#ifndef SOLVERDEFINITIONINTEGEROPTIONCELLGRIDRELATEDCONDITION_H
#define SOLVERDEFINITIONINTEGEROPTIONCELLGRIDRELATEDCONDITION_H

#include "solverdefinitionintegercellgridrelatedcondition.h"
#include "enumloader.h"

class SolverDefinitionIntegerOptionCellGridRelatedCondition : public SolverDefinitionIntegerCellGridRelatedCondition, public IntegerEnumLoader
{
public:
	SolverDefinitionIntegerOptionCellGridRelatedCondition(QDomElement node, const SolverDefinitionTranslator& translator);
	GridRelatedConditionEditWidget* editWidget(QWidget* parent);
	GridRelatedConditionVariationEditWidget* variationEditWidget(QWidget* /*parent*/){return 0;}
	ScalarsToColorsEditWidget* createScalarsToColorsEditWidget(QWidget *parent);
	ScalarsToColorsContainer* createScalarsToColorsContainer(ProjectDataItem* d);
};

#endif // SOLVERDEFINITIONINTEGEROPTIONCELLGRIDRELATEDCONDITION_H
