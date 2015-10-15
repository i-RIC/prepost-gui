#ifndef SOLVERDEFINITIONREALNODEGRIDRELATEDCONDITION_H
#define SOLVERDEFINITIONREALNODEGRIDRELATEDCONDITION_H

#include "solverdefinitiongridattributet.h"

class SolverDefinitionGridAttributeRealNode : public SolverDefinitionGridAttributeReal
{

public:
	/// Constructor
	SolverDefinitionGridAttributeRealNode(QDomElement node, const SolverDefinitionTranslator& translator, int order) :
		SolverDefinitionGridAttributeReal {node, translator, Node, false, order}
	{}
	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* parent) override;

protected:
	/// Constructor
	SolverDefinitionGridAttributeRealNode(QDomElement node, const SolverDefinitionTranslator& translator, bool isOption, int order) :
		SolverDefinitionGridAttributeReal {node, translator, Node, isOption, order}
	{}

private:
	GridAttributeContainer* buildContainer(Grid* grid) override;
};

#endif // SOLVERDEFINITIONREALNODEGRIDRELATEDCONDITION_H
