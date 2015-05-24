#ifndef SOLVERDEFINITIONINTEGEROPTIONNODEGRIDRELATEDCONDITION_H
#define SOLVERDEFINITIONINTEGEROPTIONNODEGRIDRELATEDCONDITION_H

#include "solverdefinitionintegernodegridrelatedcondition.h"
#include "enumloader.h"

class SolverDefinitionIntegerOptionNodeGridRelatedCondition : public SolverDefinitionIntegerNodeGridRelatedCondition, public IntegerEnumLoader
{
public:
	SolverDefinitionIntegerOptionNodeGridRelatedCondition(QDomElement node, const SolverDefinitionTranslator& translator);
	GridRelatedConditionEditWidget* editWidget(QWidget* parent);
	GridRelatedConditionVariationEditWidget* variationEditWidget(QWidget* /*parent*/) {return 0;}
	ScalarsToColorsEditWidget* createScalarsToColorsEditWidget(QWidget* parent) override;
	ScalarsToColorsContainer* createScalarsToColorsContainer(ProjectDataItem* d) override;
};

#endif // SOLVERDEFINITIONINTEGEROPTIONNODEGRIDRELATEDCONDITION_H
