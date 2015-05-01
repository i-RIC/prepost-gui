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
	ScalarsToColorsEditWidget* createScalarsToColorsEditWidget(QWidget* parent);
	ScalarsToColorsContainer* createScalarsToColorsContainer(ProjectDataItem* d);
};

#endif // SOLVERDEFINITIONINTEGEROPTIONNODEGRIDRELATEDCONDITION_H
