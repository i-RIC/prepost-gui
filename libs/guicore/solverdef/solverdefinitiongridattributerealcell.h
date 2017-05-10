#ifndef SOLVERDEFINITIONGRIDATTRIBUTEREALCELL_H
#define SOLVERDEFINITIONGRIDATTRIBUTEREALCELL_H

#include "solverdefinitiongridattributereal.h"

class SolverDefinitionGridAttributeRealCell : public SolverDefinitionGridAttributeReal
{
public:
	SolverDefinitionGridAttributeRealCell(const QDomElement& elem, SolverDefinition* solverDef, int order);

	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* parent) override;

protected:
	SolverDefinitionGridAttributeRealCell(const QDomElement& elem, SolverDefinition *solverDef, bool isOption, int order);

private:
	GridAttributeContainer* buildContainer(Grid* grid) override;
};

#endif // SOLVERDEFINITIONGRIDATTRIBUTEREALCELL_H
