#ifndef SOLVERDEFINITIONREALOPTIONNODEGRIDRELATEDCONDITION_H
#define SOLVERDEFINITIONREALOPTIONNODEGRIDRELATEDCONDITION_H

#include "solverdefinitionrealnodegridrelatedcondition.h"
#include "enumloader.h"

class SolverDefinitionRealOptionNodeGridRelatedCondition : public SolverDefinitionRealNodeGridRelatedCondition, public RealEnumLoader
{
public:
	SolverDefinitionRealOptionNodeGridRelatedCondition(QDomElement node, const SolverDefinitionTranslator& translator);
	GridRelatedConditionEditWidget* editWidget(QWidget* parent) override;
	GridRelatedConditionVariationEditWidget* variationEditWidget(QWidget* /*parent*/) override {return 0;}
	ScalarsToColorsEditWidget* createScalarsToColorsEditWidget(QWidget* parent) override;
	ScalarsToColorsContainer* createScalarsToColorsContainer(ProjectDataItem* d) override;
};

#endif // SOLVERDEFINITIONREALOPTIONNODEGRIDRELATEDCONDITION_H
