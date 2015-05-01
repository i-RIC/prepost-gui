#ifndef GRIDRELATEDCONDITIONBASEOBJECT_H
#define GRIDRELATEDCONDITIONBASEOBJECT_H

#include <QObject>

class SolverDefinitionGridRelatedCondition;
class GridRelatedConditionBaseObject : public QObject
{
	Q_OBJECT

public:
	/// Constructor
	GridRelatedConditionBaseObject(SolverDefinitionGridRelatedCondition* cond, QObject* parent = nullptr)
		: QObject(parent) {
		m_condition = cond;
	}
	/// The grid related condition that is the ancestor
	/// of this instance.
	SolverDefinitionGridRelatedCondition* condition() const {
		return m_condition;
	}
protected:
	SolverDefinitionGridRelatedCondition* m_condition;
};

#endif // GRIDRELATEDCONDITIONBASEOBJECT_H
