#ifndef SOLVERDEFINITIONREALNODEGRIDRELATEDCONDITION_H
#define SOLVERDEFINITIONREALNODEGRIDRELATEDCONDITION_H

#include "solverdefinitiongridattributet.h"

class SolverDefinitionGridAttributeRealNode : public SolverDefinitionGridAttributeReal
{
public:
	/// Constructor
	SolverDefinitionGridAttributeRealNode(QDomElement node, const SolverDefinitionTranslator& translator)
		: SolverDefinitionGridAttributeReal(node, translator) {
		m_position = Node;
	}
	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* parent) override;
protected:
	GridAttributeContainer* buildContainer(Grid* grid) override;
};

#endif // SOLVERDEFINITIONREALNODEGRIDRELATEDCONDITION_H
