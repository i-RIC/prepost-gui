#ifndef SOLVERDEFINITIONGRIDATTRIBUTEINTEGERIFACE_H
#define SOLVERDEFINITIONGRIDATTRIBUTEINTEGERIFACE_H

#include "solverdefinitiongridattributeinteger.h"

class SolverDefinitionGridAttributeIntegerIFace : public SolverDefinitionGridAttributeInteger
{
public:
	SolverDefinitionGridAttributeIntegerIFace(const QDomElement& elem, SolverDefinition* solverDef, int order);

	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* parent) override;

protected:
	SolverDefinitionGridAttributeIntegerIFace(const QDomElement& elem, SolverDefinition* solverDef, bool isOption, int order);

private:
	GridAttributeContainer* buildContainer(Grid* grid) override;
};

#endif // SOLVERDEFINITIONGRIDATTRIBUTEINTEGERIFACE_H
