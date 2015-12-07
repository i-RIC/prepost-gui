#ifndef SOLVERDEFINITIONGRIDATTRIBUTEINTEGERNODE_H
#define SOLVERDEFINITIONGRIDATTRIBUTEINTEGERNODE_H

#include "solverdefinitiongridattributeinteger.h"

class SolverDefinitionGridAttributeIntegerNode : public SolverDefinitionGridAttributeInteger
{
public:
	SolverDefinitionGridAttributeIntegerNode(const QDomElement& elem, const SolverDefinitionTranslator& translator, int order);

	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* parent) override;

protected:
	SolverDefinitionGridAttributeIntegerNode(const QDomElement& elem, const SolverDefinitionTranslator& translator, bool isOption, int order);

private:
	GridAttributeContainer* buildContainer(Grid* grid) override;
};

#endif // SOLVERDEFINITIONGRIDATTRIBUTEINTEGERNODE_H
