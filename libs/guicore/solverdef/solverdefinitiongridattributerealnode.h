#ifndef SOLVERDEFINITIONGRIDATTRIBUTEREALNODE_H
#define SOLVERDEFINITIONGRIDATTRIBUTEREALNODE_H

#include "solverdefinitiongridattributereal.h"

class SolverDefinitionGridAttributeRealNode : public SolverDefinitionGridAttributeReal
{
public:
	SolverDefinitionGridAttributeRealNode(const QDomElement& elem, const SolverDefinitionTranslator& translator, int order);

	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* parent) override;

protected:
	SolverDefinitionGridAttributeRealNode(const QDomElement& elem, const SolverDefinitionTranslator& translator, bool isOption, int order);

private:
	GridAttributeContainer* buildContainer(Grid* grid) override;
};

#endif // SOLVERDEFINITIONGRIDATTRIBUTEINTEGERNODE_H
