#ifndef SOLVERDEFINITIONINTEGERCELLGRIDRELATEDCONDITION_H
#define SOLVERDEFINITIONINTEGERCELLGRIDRELATEDCONDITION_H

#include "solverdefinitiongridattributet.h"

class SolverDefinitionGridAttributeIntegerCell : public SolverDefinitionGridAttributeInteger
{
public:
	/// Constructor
	SolverDefinitionGridAttributeIntegerCell(QDomElement node, const SolverDefinitionTranslator& translator)
		: SolverDefinitionGridAttributeInteger(node, translator) {
		m_position = CellCenter;
	}
	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* parent) override;
protected:
	GridAttributeContainer* buildContainer(Grid* grid) override;
};

#endif // SOLVERDEFINITIONINTEGERCELLGRIDRELATEDCONDITION_H
