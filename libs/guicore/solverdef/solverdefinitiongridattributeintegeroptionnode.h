#ifndef SOLVERDEFINITIONINTEGEROPTIONNODEGRIDRELATEDCONDITION_H
#define SOLVERDEFINITIONINTEGEROPTIONNODEGRIDRELATEDCONDITION_H

#include "solverdefinitiongridattributeintegernode.h"
#include "enumloader.h"

class SolverDefinitionGridAttributeIntegerOptionNode : public SolverDefinitionGridAttributeIntegerNode, public IntegerEnumLoader
{

public:
	SolverDefinitionGridAttributeIntegerOptionNode(QDomElement node, const SolverDefinitionTranslator& translator);
	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* /*parent*/) override {return 0;}
	ScalarsToColorsEditWidget* createScalarsToColorsEditWidget(QWidget* parent) const override;
	ScalarsToColorsContainer* createScalarsToColorsContainer(ProjectDataItem* d) override;
};

#endif // SOLVERDEFINITIONINTEGEROPTIONNODEGRIDRELATEDCONDITION_H
