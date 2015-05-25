#ifndef SOLVERDEFINITIONREALCELLGRIDRELATEDCONDITION_H
#define SOLVERDEFINITIONREALCELLGRIDRELATEDCONDITION_H

#include "solverdefinitiongridrelatedconditiont.h"

class SolverDefinitionRealCellGridRelatedCondition : public SolverDefinitionGridRelatedRealCondition
{
public:
	/// Constructor
	SolverDefinitionRealCellGridRelatedCondition(QDomElement node, const SolverDefinitionTranslator& translator)
		: SolverDefinitionGridRelatedRealCondition(node, translator) {
		m_position = CellCenter;
	}
	GridRelatedConditionEditWidget* editWidget(QWidget* parent) override;
	GridRelatedConditionVariationEditWidget* variationEditWidget(QWidget* parent) override;
protected:
	GridRelatedConditionContainer* buildContainer(Grid* grid) override;
};

#endif // SOLVERDEFINITIONREALCELLGRIDRELATEDCONDITION_H
