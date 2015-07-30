#ifndef SOLVERDEFINITIONINTEGERNODEGRIDRELATEDCONDITION_H
#define SOLVERDEFINITIONINTEGERNODEGRIDRELATEDCONDITION_H

#include "solverdefinitiongridattributet.h"

class SolverDefinitionGridAttributeIntegerNode : public SolverDefinitionGridAttributeInteger
{

public:
	/// Constructor
	SolverDefinitionGridAttributeIntegerNode(QDomElement node, const SolverDefinitionTranslator& translator) :
		SolverDefinitionGridAttributeInteger {node, translator, Node, false}
	{}
	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* parent) override;

protected:
	/// Constructor
	SolverDefinitionGridAttributeIntegerNode(QDomElement node, const SolverDefinitionTranslator& translator, bool isOption) :
		SolverDefinitionGridAttributeInteger {node, translator, Node, isOption}
	{}

private:
	GridAttributeContainer* buildContainer(Grid* grid) override;
};

#endif // SOLVERDEFINITIONINTEGERNODEGRIDRELATEDCONDITION_H
