#ifndef SOLVERDEFINITIONINTEGERCELLGRIDATTRIBUTE_H
#define SOLVERDEFINITIONINTEGERCELLGRIDATTRIBUTE_H

#include "solverdefinitiongridattributet.h"

class SolverDefinitionGridAttributeIntegerCell : public SolverDefinitionGridAttributeInteger
{

public:
	/// Constructor
	SolverDefinitionGridAttributeIntegerCell(const QDomElement& elem, const SolverDefinitionTranslator& translator, int order) :
		SolverDefinitionGridAttributeInteger{ elem, translator, CellCenter, false, order }
	{}
	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* parent) override;

protected:
	/// Constructor
	SolverDefinitionGridAttributeIntegerCell(const QDomElement& elem, const SolverDefinitionTranslator& translator, bool isOption, int order) :
		SolverDefinitionGridAttributeInteger{elem, translator, CellCenter, isOption, order }
	{}

private:
	GridAttributeContainer* buildContainer(Grid* grid) override;
};

#endif // SOLVERDEFINITIONINTEGERCELLGRIDATTRIBUTE_H
