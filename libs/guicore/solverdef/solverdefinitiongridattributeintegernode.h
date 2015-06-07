#ifndef SOLVERDEFINITIONINTEGERNODEGRIDRELATEDCONDITION_H
#define SOLVERDEFINITIONINTEGERNODEGRIDRELATEDCONDITION_H

#include "solverdefinitiongridattributet.h"

class SolverDefinitionGridAttributeIntegerNode : public SolverDefinitionGridAttributeInteger
{

public:
	/// Constructor
	SolverDefinitionGridAttributeIntegerNode(QDomElement node, const SolverDefinitionTranslator& translator)
		: SolverDefinitionGridAttributeInteger(node, translator) {
		m_position = Node;
	}
	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* parent) override;

protected:
	GridAttributeContainer* buildContainer(Grid* grid) override;
};

#endif // SOLVERDEFINITIONINTEGERNODEGRIDRELATEDCONDITION_H
