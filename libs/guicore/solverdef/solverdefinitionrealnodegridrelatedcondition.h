#ifndef SOLVERDEFINITIONREALNODEGRIDRELATEDCONDITION_H
#define SOLVERDEFINITIONREALNODEGRIDRELATEDCONDITION_H

#include "solverdefinitiongridrelatedconditiont.h"

class SolverDefinitionRealNodeGridRelatedCondition : public SolverDefinitionGridRelatedRealCondition
{
public:
	/// Constructor
	SolverDefinitionRealNodeGridRelatedCondition(QDomElement node, const SolverDefinitionTranslator& translator)
		: SolverDefinitionGridRelatedRealCondition(node, translator) {
		m_position = Node;
	}
	GridRelatedConditionEditWidget* editWidget(QWidget* parent) override;
	GridRelatedConditionVariationEditWidget* variationEditWidget(QWidget* parent) override;
protected:
	GridRelatedConditionContainer* buildContainer(Grid* grid) override;
};

#endif // SOLVERDEFINITIONREALNODEGRIDRELATEDCONDITION_H
