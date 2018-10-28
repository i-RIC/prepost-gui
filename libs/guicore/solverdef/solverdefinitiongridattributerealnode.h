#ifndef SOLVERDEFINITIONGRIDATTRIBUTEREALNODE_H
#define SOLVERDEFINITIONGRIDATTRIBUTEREALNODE_H

#include "../guicore_global.h"

#include "solverdefinitiongridattributereal.h"

class GUICOREDLL_EXPORT SolverDefinitionGridAttributeRealNode : public SolverDefinitionGridAttributeReal
{
public:
	SolverDefinitionGridAttributeRealNode(const QDomElement& elem, SolverDefinition* solverDef, int order);

	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* parent) override;

protected:
	SolverDefinitionGridAttributeRealNode(const QDomElement& elem, SolverDefinition* solverDef, bool isOption, int order);

private:
	GridAttributeContainer* buildContainer(Grid* grid) override;
};

#endif // SOLVERDEFINITIONGRIDATTRIBUTEINTEGERNODE_H
