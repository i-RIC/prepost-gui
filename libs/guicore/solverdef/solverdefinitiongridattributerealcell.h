#ifndef SOLVERDEFINITIONREALCELLGRIDRELATEDCONDITION_H
#define SOLVERDEFINITIONREALCELLGRIDRELATEDCONDITION_H

#include "solverdefinitiongridattributet.h"

class SolverDefinitionGridAttributeRealCell : public SolverDefinitionGridAttributeReal
{

public:
	/// Constructor
	SolverDefinitionGridAttributeRealCell(QDomElement node, const SolverDefinitionTranslator& translator) :
		SolverDefinitionGridAttributeReal {node, translator, CellCenter, false}
	{}
	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* parent) override;

protected:
	/// Constructor
	SolverDefinitionGridAttributeRealCell(QDomElement node, const SolverDefinitionTranslator& translator, bool isOption) :
		SolverDefinitionGridAttributeReal {node, translator, CellCenter, isOption}
	{}

private:
	GridAttributeContainer* buildContainer(Grid* grid) override;
};

#endif // SOLVERDEFINITIONREALCELLGRIDRELATEDCONDITION_H
