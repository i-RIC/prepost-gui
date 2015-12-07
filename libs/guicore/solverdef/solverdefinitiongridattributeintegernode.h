#ifndef SOLVERDEFINITIONGRIDATTRIBUTEINTEGERNODE_H
#define SOLVERDEFINITIONGRIDATTRIBUTEINTEGERNODE_H

#include "solverdefinitiongridattributet.h"

class SolverDefinitionGridAttributeIntegerNode : public SolverDefinitionGridAttributeInteger
{

public:
	/// Constructor
	SolverDefinitionGridAttributeIntegerNode(const QDomElement& elem, const SolverDefinitionTranslator& translator, int order) :
		SolverDefinitionGridAttributeInteger {elem, translator, Node, false, order}
	{}
	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* parent) override;

protected:
	/// Constructor
	SolverDefinitionGridAttributeIntegerNode(const QDomElement& elem, const SolverDefinitionTranslator& translator, bool isOption, int order) :
		SolverDefinitionGridAttributeInteger {elem, translator, Node, isOption, order}
	{}

private:
	GridAttributeContainer* buildContainer(Grid* grid) override;
};

#endif // SOLVERDEFINITIONGRIDATTRIBUTEINTEGERNODE_H
