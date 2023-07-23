#ifndef SOLVERDEFINITIONGRIDATTRIBUTEREALJFACE_H
#define SOLVERDEFINITIONGRIDATTRIBUTEREALJFACE_H

#include "solverdefinitiongridattributereal.h"

class SolverDefinitionGridAttributeRealJFace : public SolverDefinitionGridAttributeReal
{
public:
	SolverDefinitionGridAttributeRealJFace(const QDomElement& elem, SolverDefinition* solverDef, int order);

	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* parent) override;

protected:
	SolverDefinitionGridAttributeRealJFace(const QDomElement& elem, SolverDefinition *solverDef, bool isOption, int order);

private:
	GridAttributeContainer* buildContainer(Grid* grid) override;
};

#endif // SOLVERDEFINITIONGRIDATTRIBUTEREALJFACE_H
