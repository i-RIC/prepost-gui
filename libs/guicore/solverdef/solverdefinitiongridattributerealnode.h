#ifndef SOLVERDEFINITIONGRIDATTRIBUTEREALNODE_H
#define SOLVERDEFINITIONGRIDATTRIBUTEREALNODE_H

#include "solverdefinitiongridattributet.h"

class SolverDefinitionGridAttributeRealNode : public SolverDefinitionGridAttributeReal
{

public:
	/// Constructor
	SolverDefinitionGridAttributeRealNode(const QDomElement& elem, const SolverDefinitionTranslator& translator, int order) :
		SolverDefinitionGridAttributeReal {elem, translator, Node, false, order}
	{}
	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* parent) override;

protected:
	/// Constructor
	SolverDefinitionGridAttributeRealNode(const QDomElement& elem, const SolverDefinitionTranslator& translator, bool isOption, int order) :
		SolverDefinitionGridAttributeReal {elem, translator, Node, isOption, order}
	{}

private:
	GridAttributeContainer* buildContainer(Grid* grid) override;
};

#endif // SOLVERDEFINITIONGRIDATTRIBUTEINTEGERNODE_H
