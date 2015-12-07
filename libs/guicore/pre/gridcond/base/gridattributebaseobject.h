#ifndef GRIDATTRIBUTEBASEOBJECT_H
#define GRIDATTRIBUTEBASEOBJECT_H

#include <QObject>

class SolverDefinitionGridAttribute;
class GridAttributeBaseObject : public QObject
{
	Q_OBJECT

public:
	/// Constructor
	GridAttributeBaseObject(SolverDefinitionGridAttribute* cond, QObject* parent = nullptr) :
		QObject {parent},
		m_condition {cond}
	{}
	/// The grid related condition that is the ancestor
	/// of this instance.
	SolverDefinitionGridAttribute* condition() const {return m_condition;}

protected:
	SolverDefinitionGridAttribute* m_condition;
};

#endif // GRIDATTRIBUTEBASEOBJECT_H
