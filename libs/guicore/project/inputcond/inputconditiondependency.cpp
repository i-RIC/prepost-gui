#include "inputconditioncontainerset.h"
#include "inputconditiondependency.h"

#include <misc/errormessage.h>

#include <QList>

InputConditionDependency::InputConditionDependency()
{
	m_condition = 0;
}

InputConditionDependency::Condition::Condition(InputConditionDependency* c)
{
	c->setCondition(this);
}

// Definitions of classes those inherit CgnsFileInputConditionDependency::Action

InputConditionDependency::Action* InputConditionDependency::buildAction(
	const QDomNode& /*node*/,
	InputConditionContainerSet* /*cs*/,
	InputConditionWidgetSet* /*ws*/,
	InputConditionWidget* /*w*/
)
{
	// @todo not implemented yet.
	return 0;
}
// Definitions of classes those inherit CgnsFileInputConditionDependency::Condition

class CgnsFileInputConditionDependencyConditionAlways : public InputConditionDependency::Condition
{
public:
	/// Constructor
	CgnsFileInputConditionDependencyConditionAlways(const QDomNode& node, InputConditionContainerSet* cs, InputConditionDependency* dep)
		: InputConditionDependency::Condition(dep) {
		QString targName = node.toElement().attribute("target");
		InputConditionContainer* target = cs->container(targName);
		QObject::connect(target, SIGNAL(valueChanged()), dep, SLOT(check()));
	}
	/// Destructor
	~CgnsFileInputConditionDependencyConditionAlways() {}
	bool match() {return true;}
};

class CgnsFileInputConditionDependencyConditionNot : public InputConditionDependency::Condition
{
public:
	/// Constructor
	CgnsFileInputConditionDependencyConditionNot(const QDomNode& node, InputConditionContainerSet* cs, InputConditionDependency* dep)
		: InputConditionDependency::Condition(dep) {
		m_child = nullptr;
		const QDomNodeList& list = node.childNodes();
		for (int i = 0; i < list.count(); ++i) {
			m_child = InputConditionDependency::buildCondition(list.item(i), cs, dep);
			// use the first child only.
			break;
		}
	}
	/// Destructor
	~CgnsFileInputConditionDependencyConditionNot() {
		delete m_child;
	}

	bool match() {
		return ! m_child->match();
	}
private:
	InputConditionDependency::Condition* m_child;
};

class CgnsFileInputConditionDependencyConditionAnd : public InputConditionDependency::Condition
{
public:
	/// Constructor
	CgnsFileInputConditionDependencyConditionAnd(const QDomNode& node, InputConditionContainerSet* cs, InputConditionDependency* dep)
		: InputConditionDependency::Condition(dep) {
		const QDomNodeList& list = node.childNodes();
		for (int i = 0; i < list.count(); ++i) {
			InputConditionDependency::Condition* child = InputConditionDependency::buildCondition(list.item(i), cs, dep);
			m_children.append(child);
		}
	}
	/// Destructor
	~CgnsFileInputConditionDependencyConditionAnd() {
		for (auto c : m_children) {
			delete c;
		}
	}
	bool match() {
		bool ret = true;
		for (auto c : m_children) {
			ret = ret && c->match();
		}
		return ret;
	}
private:
	QList<InputConditionDependency::Condition*> m_children;
};

class CgnsFileInputConditionDependencyConditionOr : public InputConditionDependency::Condition
{
public:
	/// Constructor
	CgnsFileInputConditionDependencyConditionOr(const QDomNode& node, InputConditionContainerSet* cs, InputConditionDependency* dep)
		: InputConditionDependency::Condition(dep) {
		const QDomNodeList& list = node.childNodes();
		for (int i = 0; i < list.count(); ++i) {
			InputConditionDependency::Condition* child = InputConditionDependency::buildCondition(list.item(i), cs, dep);
			m_children.append(child);
		}
	}
	/// Destructor
	~CgnsFileInputConditionDependencyConditionOr() {
		for (auto c : m_children) {
			delete c;
		}
	}
	bool match() {
		bool ret = false;
		for (auto c : m_children) {
			ret = ret || c->match();
		}
		return ret;
	}
private:
	QList<InputConditionDependency::Condition*> m_children;

};

class CgnsFileInputConditionDependencyConditionIsEqual : public InputConditionDependency::Condition
{
public:
	/// Constructor
	CgnsFileInputConditionDependencyConditionIsEqual(const QDomNode& node, InputConditionContainerSet* cs, InputConditionDependency* dep)
		: InputConditionDependency::Condition(dep) {
		QDomElement elem = node.toElement();
		QString targName = elem.attribute("target");
		QString valueStr = elem.attribute("value");
		m_target = cs->container(targName);
		if (m_target == 0) {
			// target is not correctly specified.
			throw ErrorMessage("target is not correctly specified.");
		}
		m_value = QVariant(valueStr);
		// signal/slot connection
		QObject::connect(m_target, SIGNAL(valueChanged()), dep, SLOT(check()));
	}
	/// Destructor
	~CgnsFileInputConditionDependencyConditionIsEqual() {}
	bool match() {
		QVariant currentValue = m_target->variantValue();
		return currentValue == m_value;
	}
private:
	InputConditionContainer* m_target;
	QVariant m_value;
};
class CgnsFileInputConditionDependencyConditionIsGreaterEqual : public InputConditionDependency::Condition
{
public:
	/// Constructor
	CgnsFileInputConditionDependencyConditionIsGreaterEqual(const QDomNode& node, InputConditionContainerSet* cs, InputConditionDependency* dep)
		: InputConditionDependency::Condition(dep) {
		QDomElement elem = node.toElement();
		QString targName = elem.attribute("target");
		QString valueStr = elem.attribute("value");
		m_target = cs->container(targName);
		m_value = QVariant(valueStr);
		// signal/slot connection
		QObject::connect(m_target, SIGNAL(valueChanged()), dep, SLOT(check()));
	}
	/// Destructor
	~CgnsFileInputConditionDependencyConditionIsGreaterEqual() {}
	bool match() {
		QVariant currentValue = m_target->variantValue();
		return currentValue.toDouble() >= m_value.toDouble();
	}
private:
	InputConditionContainer* m_target;
	QVariant m_value;
};

class CgnsFileInputConditionDependencyConditionIsGreaterThan : public InputConditionDependency::Condition
{
public:
	/// Constructor
	CgnsFileInputConditionDependencyConditionIsGreaterThan(const QDomNode& node, InputConditionContainerSet* cs, InputConditionDependency* dep)
		: InputConditionDependency::Condition(dep) {
		QDomElement elem = node.toElement();
		QString targName = elem.attribute("target");
		QString valueStr = elem.attribute("value");
		m_target = cs->container(targName);
		m_value = QVariant(valueStr);
		// signal/slot connection
		QObject::connect(m_target, SIGNAL(valueChanged()), dep, SLOT(check()));
	}
	/// Destructor
	~CgnsFileInputConditionDependencyConditionIsGreaterThan() {}
	bool match() {
		QVariant currentValue = m_target->variantValue();
		return currentValue.toDouble() > m_value.toDouble();
	}
private:
	InputConditionContainer* m_target;
	QVariant m_value;
};

class CgnsFileInputConditionDependencyConditionIsLessEqual : public InputConditionDependency::Condition
{
public:
	/// Constructor
	CgnsFileInputConditionDependencyConditionIsLessEqual(const QDomNode& node, InputConditionContainerSet* cs, InputConditionDependency* dep)
		: InputConditionDependency::Condition(dep) {
		QDomElement elem = node.toElement();
		QString targName = elem.attribute("target");
		QString valueStr = elem.attribute("value");
		m_target = cs->container(targName);
		m_value = QVariant(valueStr);
		// signal/slot connection
		QObject::connect(m_target, SIGNAL(valueChanged()), dep, SLOT(check()));
	}
	/// Destructor
	~CgnsFileInputConditionDependencyConditionIsLessEqual() {}
	bool match() {
		QVariant currentValue = m_target->variantValue();
		return currentValue.toDouble() <= m_value.toDouble();
	}
private:
	InputConditionContainer* m_target;
	QVariant m_value;
};

class CgnsFileInputConditionDependencyConditionIsLessThan : public InputConditionDependency::Condition
{
public:
	/// Constructor
	CgnsFileInputConditionDependencyConditionIsLessThan(const QDomNode& node, InputConditionContainerSet* cs, InputConditionDependency* dep)
		: InputConditionDependency::Condition(dep) {
		QDomElement elem = node.toElement();
		QString targName = elem.attribute("target");
		QString valueStr = elem.attribute("value");
		m_target = cs->container(targName);
		m_value = QVariant(valueStr);
		// signal/slot connection
		QObject::connect(m_target, SIGNAL(valueChanged()), dep, SLOT(check()));
	}
	/// Destructor
	~CgnsFileInputConditionDependencyConditionIsLessThan() {}
	bool match() {
		QVariant currentValue = m_target->variantValue();
		return currentValue.toDouble() < m_value.toDouble();
	}
private:
	InputConditionContainer* m_target;
	QVariant m_value;
};

InputConditionDependency::Condition* InputConditionDependency::buildCondition(
	const QDomNode& node,
	InputConditionContainerSet* cs,
	InputConditionDependency* d
)
{
	InputConditionDependency::Condition* ret = nullptr;
	QString type = node.toElement().attribute("type");
	if (type == "always") {
		ret = new CgnsFileInputConditionDependencyConditionAlways(node, cs, d);
	} else if (type == "isEqual") {
		ret = new CgnsFileInputConditionDependencyConditionIsEqual(node, cs, d);
	} else if (type == "isGreaterEqual") {
		ret = new CgnsFileInputConditionDependencyConditionIsGreaterEqual(node, cs, d);
	} else if (type == "isGreaterThan") {
		ret = new CgnsFileInputConditionDependencyConditionIsGreaterThan(node, cs, d);
	} else if (type == "isLessEqual") {
		ret = new CgnsFileInputConditionDependencyConditionIsLessEqual(node, cs, d);
	} else if (type == "isLessThan") {
		ret = new CgnsFileInputConditionDependencyConditionIsLessThan(node, cs, d);
	} else if (type == "and") {
		ret = new CgnsFileInputConditionDependencyConditionAnd(node, cs, d);
	} else if (type == "or") {
		ret = new CgnsFileInputConditionDependencyConditionOr(node, cs, d);
	} else if (type == "not") {
		ret = new CgnsFileInputConditionDependencyConditionNot(node, cs, d);
	} else {
		throw ErrorMessage(QString("Unknown condition type %1").arg(type));
	}
	return ret;
}
