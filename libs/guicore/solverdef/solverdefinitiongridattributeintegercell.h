#ifndef SOLVERDEFINITIONINTEGERCELLGRIDATTRIBUTE_H
#define SOLVERDEFINITIONINTEGERCELLGRIDATTRIBUTE_H

#include "solverdefinitiongridattributeinteger.h"

class SolverDefinitionGridAttributeIntegerCell : public SolverDefinitionGridAttributeInteger
{
public:
	SolverDefinitionGridAttributeIntegerCell(const QDomElement& elem, SolverDefinition* solverDef, int order);

	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* parent) override;

protected:
	SolverDefinitionGridAttributeIntegerCell(const QDomElement& elem, SolverDefinition* solverDef, bool isOption, int order);

private:
	GridAttributeContainer* buildContainer(Grid* grid) override;
};

#endif // SOLVERDEFINITIONINTEGERCELLGRIDATTRIBUTE_H
