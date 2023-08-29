#ifndef SOLVERDEFINITIONGRIDATTRIBUTEREALIFACE_H
#define SOLVERDEFINITIONGRIDATTRIBUTEREALIFACE_H

#include "solverdefinitiongridattributereal.h"

class SolverDefinitionGridAttributeRealIFace : public SolverDefinitionGridAttributeReal
{
public:
	SolverDefinitionGridAttributeRealIFace(const QDomElement& elem, SolverDefinition* solverDef, int order);

	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* parent) override;

protected:
	SolverDefinitionGridAttributeRealIFace(const QDomElement& elem, SolverDefinition *solverDef, bool isOption, int order);

private:
	GridAttributeContainer* buildContainer(Grid* grid) override;
};

#endif // SOLVERDEFINITIONGRIDATTRIBUTEREALIFACE_H
