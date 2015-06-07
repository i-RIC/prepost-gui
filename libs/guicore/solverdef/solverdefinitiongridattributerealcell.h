#ifndef SOLVERDEFINITIONREALCELLGRIDRELATEDCONDITION_H
#define SOLVERDEFINITIONREALCELLGRIDRELATEDCONDITION_H

#include "solverdefinitiongridattributet.h"

class SolverDefinitionGridAttributeRealCell : public SolverDefinitionGridAttributeReal
{
public:
	/// Constructor
	SolverDefinitionGridAttributeRealCell(QDomElement node, const SolverDefinitionTranslator& translator)
		: SolverDefinitionGridAttributeReal(node, translator) {
		m_position = CellCenter;
	}
	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* parent) override;
protected:
	GridAttributeContainer* buildContainer(Grid* grid) override;
};

#endif // SOLVERDEFINITIONREALCELLGRIDRELATEDCONDITION_H
