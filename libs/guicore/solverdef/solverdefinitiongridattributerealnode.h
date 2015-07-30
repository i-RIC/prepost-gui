#ifndef SOLVERDEFINITIONREALNODEGRIDRELATEDCONDITION_H
#define SOLVERDEFINITIONREALNODEGRIDRELATEDCONDITION_H

#include "solverdefinitiongridattributet.h"

class SolverDefinitionGridAttributeRealNode : public SolverDefinitionGridAttributeReal
{

public:
	/// Constructor
	SolverDefinitionGridAttributeRealNode(QDomElement node, const SolverDefinitionTranslator& translator) :
		SolverDefinitionGridAttributeReal {node, translator, Node, false}
	{}
	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* parent) override;

protected:
	/// Constructor
	SolverDefinitionGridAttributeRealNode(QDomElement node, const SolverDefinitionTranslator& translator, bool isOption) :
		SolverDefinitionGridAttributeReal {node, translator, Node, isOption}
	{}

private:
	GridAttributeContainer* buildContainer(Grid* grid) override;
};

#endif // SOLVERDEFINITIONREALNODEGRIDRELATEDCONDITION_H
