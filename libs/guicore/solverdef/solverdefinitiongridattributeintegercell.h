#ifndef SOLVERDEFINITIONINTEGERCELLGRIDATTRIBUTE_H
#define SOLVERDEFINITIONINTEGERCELLGRIDATTRIBUTE_H

#include "solverdefinitiongridattributeinteger.h"

class SolverDefinitionGridAttributeIntegerCell : public SolverDefinitionGridAttributeInteger
{

public:
	SolverDefinitionGridAttributeIntegerCell(const QDomElement& elem, const SolverDefinitionTranslator& translator, int order);

	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* parent) override;

protected:
	SolverDefinitionGridAttributeIntegerCell(const QDomElement& elem, const SolverDefinitionTranslator& translator, bool isOption, int order);

private:
	GridAttributeContainer* buildContainer(Grid* grid) override;
};

#endif // SOLVERDEFINITIONINTEGERCELLGRIDATTRIBUTE_H
