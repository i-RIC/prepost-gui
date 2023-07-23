#ifndef SOLVERDEFINITIONGRIDATTRIBUTEINTEGERJFACE_H
#define SOLVERDEFINITIONGRIDATTRIBUTEINTEGERJFACE_H

#include "solverdefinitiongridattributeinteger.h"

class SolverDefinitionGridAttributeIntegerJFace : public SolverDefinitionGridAttributeInteger
{
public:
	SolverDefinitionGridAttributeIntegerJFace(const QDomElement& elem, SolverDefinition* solverDef, int order);

	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* parent) override;

protected:
	SolverDefinitionGridAttributeIntegerJFace(const QDomElement& elem, SolverDefinition* solverDef, bool isOption, int order);

private:
	GridAttributeContainer* buildContainer(Grid* grid) override;
};

#endif // SOLVERDEFINITIONGRIDATTRIBUTEINTEGERJFACE_H
