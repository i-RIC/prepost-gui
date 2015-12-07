#ifndef SOLVERDEFINITIONGRIDATTRIBUTEREALCELL_H
#define SOLVERDEFINITIONGRIDATTRIBUTEREALCELL_H

#include "solverdefinitiongridattributet.h"

class SolverDefinitionGridAttributeRealCell : public SolverDefinitionGridAttributeReal
{

public:
	SolverDefinitionGridAttributeRealCell(const QDomElement& elem, const SolverDefinitionTranslator& translator, int order);

	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* parent) override;

protected:
	SolverDefinitionGridAttributeRealCell(const QDomElement& elem, const SolverDefinitionTranslator& translator, bool isOption, int order);

private:
	GridAttributeContainer* buildContainer(Grid* grid) override;
};

#endif // SOLVERDEFINITIONGRIDATTRIBUTEREALCELL_H
