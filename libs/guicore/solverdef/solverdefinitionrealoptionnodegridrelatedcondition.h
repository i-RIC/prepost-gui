#ifndef SOLVERDEFINITIONREALOPTIONNODEGRIDRELATEDCONDITION_H
#define SOLVERDEFINITIONREALOPTIONNODEGRIDRELATEDCONDITION_H

#include "solverdefinitionrealnodegridrelatedcondition.h"
#include "enumloader.h"

class SolverDefinitionRealOptionNodeGridRelatedCondition : public SolverDefinitionRealNodeGridRelatedCondition, public RealEnumLoader
{
public:
	SolverDefinitionRealOptionNodeGridRelatedCondition(QDomElement node, const SolverDefinitionTranslator& translator);
	GridRelatedConditionEditWidget* editWidget(QWidget* parent);
	GridRelatedConditionVariationEditWidget* variationEditWidget(QWidget* /*parent*/){return 0;}
	ScalarsToColorsEditWidget* createScalarsToColorsEditWidget(QWidget *parent);
	ScalarsToColorsContainer* createScalarsToColorsContainer(ProjectDataItem* d);
};

#endif // SOLVERDEFINITIONREALOPTIONNODEGRIDRELATEDCONDITION_H
