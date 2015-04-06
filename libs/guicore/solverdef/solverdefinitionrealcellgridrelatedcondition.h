#ifndef SOLVERDEFINITIONREALCELLGRIDRELATEDCONDITION_H
#define SOLVERDEFINITIONREALCELLGRIDRELATEDCONDITION_H

#include "solverdefinitiongridrelatedconditiont.h"

class SolverDefinitionRealCellGridRelatedCondition : public SolverDefinitionGridRelatedRealCondition
{
public:
	/// Constructor
	SolverDefinitionRealCellGridRelatedCondition(QDomElement node, const SolverDefinitionTranslator& translator)
		: SolverDefinitionGridRelatedRealCondition(node, translator)
	{
		m_position = CellCenter;
	}
	GridRelatedConditionEditWidget* editWidget(QWidget* parent);
	GridRelatedConditionVariationEditWidget* variationEditWidget(QWidget* parent);
protected:
	GridRelatedConditionContainer* buildContainer(Grid* grid);
};

#endif // SOLVERDEFINITIONREALCELLGRIDRELATEDCONDITION_H
