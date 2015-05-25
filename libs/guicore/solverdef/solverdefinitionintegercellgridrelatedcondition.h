#ifndef SOLVERDEFINITIONINTEGERCELLGRIDRELATEDCONDITION_H
#define SOLVERDEFINITIONINTEGERCELLGRIDRELATEDCONDITION_H

#include "solverdefinitiongridrelatedconditiont.h"

class SolverDefinitionIntegerCellGridRelatedCondition : public SolverDefinitionGridRelatedIntegerCondition
{
public:
	/// Constructor
	SolverDefinitionIntegerCellGridRelatedCondition(QDomElement node, const SolverDefinitionTranslator& translator)
		: SolverDefinitionGridRelatedIntegerCondition(node, translator) {
		m_position = CellCenter;
	}
	GridRelatedConditionEditWidget* editWidget(QWidget* parent) override;
	GridRelatedConditionVariationEditWidget* variationEditWidget(QWidget* parent) override;
protected:
	GridRelatedConditionContainer* buildContainer(Grid* grid) override;
};

#endif // SOLVERDEFINITIONINTEGERCELLGRIDRELATEDCONDITION_H
