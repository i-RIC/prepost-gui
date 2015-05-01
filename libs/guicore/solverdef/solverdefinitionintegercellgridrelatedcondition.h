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
	GridRelatedConditionEditWidget* editWidget(QWidget* parent);
	GridRelatedConditionVariationEditWidget* variationEditWidget(QWidget* parent);
protected:
	GridRelatedConditionContainer* buildContainer(Grid* grid);
};

#endif // SOLVERDEFINITIONINTEGERCELLGRIDRELATEDCONDITION_H
