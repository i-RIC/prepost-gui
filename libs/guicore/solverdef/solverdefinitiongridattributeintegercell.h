#ifndef SOLVERDEFINITIONINTEGERCELLGRIDRELATEDCONDITION_H
#define SOLVERDEFINITIONINTEGERCELLGRIDRELATEDCONDITION_H

#include "solverdefinitiongridattributet.h"

class SolverDefinitionGridAttributeIntegerCell : public SolverDefinitionGridAttributeInteger
{

public:
	/// Constructor
	SolverDefinitionGridAttributeIntegerCell(QDomElement node, const SolverDefinitionTranslator& translator) :
		SolverDefinitionGridAttributeInteger {node, translator, CellCenter, false}
	{}
	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* parent) override;

protected:
	/// Constructor
	SolverDefinitionGridAttributeIntegerCell(QDomElement node, const SolverDefinitionTranslator& translator, bool isOption) :
		SolverDefinitionGridAttributeInteger {node, translator, CellCenter, isOption}
	{}

private:
	GridAttributeContainer* buildContainer(Grid* grid) override;
};

#endif // SOLVERDEFINITIONINTEGERCELLGRIDRELATEDCONDITION_H
