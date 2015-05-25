#ifndef SOLVERDEFINITIONINTEGERNODEGRIDRELATEDCONDITION_H
#define SOLVERDEFINITIONINTEGERNODEGRIDRELATEDCONDITION_H

#include "solverdefinitiongridrelatedconditiont.h"

class SolverDefinitionIntegerNodeGridRelatedCondition : public SolverDefinitionGridRelatedIntegerCondition
{
public:
	/// Constructor
	SolverDefinitionIntegerNodeGridRelatedCondition(QDomElement node, const SolverDefinitionTranslator& translator)
		: SolverDefinitionGridRelatedIntegerCondition(node, translator) {
		m_position = Node;
	}
	GridRelatedConditionEditWidget* editWidget(QWidget* parent) override;
	GridRelatedConditionVariationEditWidget* variationEditWidget(QWidget* parent) override;
protected:
	GridRelatedConditionContainer* buildContainer(Grid* grid) override;
};

#endif // SOLVERDEFINITIONINTEGERNODEGRIDRELATEDCONDITION_H
