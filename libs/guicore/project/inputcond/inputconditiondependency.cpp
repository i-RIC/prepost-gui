#include "inputconditioncontainerset.h"
#include "inputconditiondependency.h"

#include <misc/errormessage.h>
#include <misc/stringtool.h>

#include <QList>

InputConditionDependency::Action::Action(InputConditionWidget *w) :
	m_target {w}
{}

InputConditionDependency::Action::~Action()
{}

InputConditionDependency::Condition::Condition(InputConditionDependency* c)
{
	c->setCondition(this);
}

InputConditionDependency::ActionEnable::ActionEnable(InputConditionWidget* w) :
	Action {w}
{}

InputConditionDependency::ActionEnable::~ActionEnable()
{}

void InputConditionDependency::ActionEnable::positiveAction()
{
	m_target->setDisabled(false);
}

void InputConditionDependency::ActionEnable::negativeAction()
{
	m_target->setDisabled(true);
}

InputConditionDependency::Condition::~Condition()
{}

// Definitions of classes those inherit InputConditionDependency::Action

InputConditionDependency::Action* InputConditionDependency::buildAction(
	const QDomNode& /*node*/,
	InputConditionContainerSet* /*cs*/,
	InputConditionWidgetSet* /*ws*/,
	InputConditionWidget* /*w*/
)
{
	// @todo not implemented yet.
	return nullptr;
}

InputConditionDependency::InputConditionDependency(QObject *parent) :
	QObject {parent},
	m_condition {nullptr}
{}

void InputConditionDependency::setCondition(Condition* c)
{
	m_condition = c;
}

void InputConditionDependency::addAction(Action* a)
{
	m_actions.push_back(a);
}

void InputConditionDependency::check()
{
	if (m_condition->match()) {
		for (auto a : m_actions) {
			a->positiveAction();
		}
	} else {
		for (auto a : m_actions) {
			a->negativeAction();
		}
	}
}

// Definitions of classes those inherit InputConditionDependency::Condition

class InputConditionDependencyConditionAlways : public InputConditionDependency::Condition
{
public:
	/// Constructor
	InputConditionDependencyConditionAlways(const QDomNode& node, InputConditionContainerSet* cs, InputConditionDependency* dep) :
		InputConditionDependency::Condition(dep)
	{
		std::string targName = iRIC::toStr(node.toElement().attribute("target"));
		InputConditionContainer* target = cs->container(targName);
		QObject::connect(target, SIGNAL(valueChanged()), dep, SLOT(check()));
	}
	/// Destructor
	~InputConditionDependencyConditionAlways() {}
	bool match() {return true;}
};

class InputConditionDependencyConditionNot : public InputConditionDependency::Condition
{
public:
	/// Constructor
	InputConditionDependencyConditionNot(const QDomNode& node, InputConditionContainerSet* cs, InputConditionDependency* dep) :
		InputConditionDependency::Condition(dep)
	{
		m_child = nullptr;
		const QDomNodeList& list = node.childNodes();
		for (int i = 0; i < list.count(); ++i) {
			m_child = InputConditionDependency::buildCondition(list.item(i), cs, dep);
			// use the first child only.
			break;
		}
	}
	/// Destructor
	~InputConditionDependencyConditionNot() {
		delete m_child;
	}

	bool match() {
		return ! m_child->match();
	}
private:
	InputConditionDependency::Condition* m_child;
};

class InputConditionDependencyConditionAnd : public InputConditionDependency::Condition
{
public:
	/// Constructor
	InputConditionDependencyConditionAnd(const QDomNode& node, InputConditionContainerSet* cs, InputConditionDependency* dep)
		: InputConditionDependency::Condition(dep) {
		const QDomNodeList& list = node.childNodes();
		for (int i = 0; i < list.count(); ++i) {
			InputConditionDependency::Condition* child = InputConditionDependency::buildCondition(list.item(i), cs, dep);
			m_children.append(child);
		}
	}
	/// Destructor
	~InputConditionDependencyConditionAnd() {
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

class InputConditionDependencyConditionOr : public InputConditionDependency::Condition
{
public:
	/// Constructor
	InputConditionDependencyConditionOr(const QDomNode& node, InputConditionContainerSet* cs, InputConditionDependency* dep)
		: InputConditionDependency::Condition(dep) {
		const QDomNodeList& list = node.childNodes();
		for (int i = 0; i < list.count(); ++i) {
			InputConditionDependency::Condition* child = InputConditionDependency::buildCondition(list.item(i), cs, dep);
			m_children.append(child);
		}
	}
	/// Destructor
	~InputConditionDependencyConditionOr() {
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

class InputConditionDependencyConditionIsEqual : public InputConditionDependency::Condition
{
public:
	/// Constructor
	InputConditionDependencyConditionIsEqual(const QDomNode& node, InputConditionContainerSet* cs, InputConditionDependency* dep) :
		InputConditionDependency::Condition(dep)
	{
		QDomElement elem = node.toElement();
		std::string targName = iRIC::toStr(elem.attribute("target"));
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
	~InputConditionDependencyConditionIsEqual() {}
	bool match() {
		QVariant currentValue = m_target->variantValue();
		return currentValue == m_value;
	}
private:
	InputConditionContainer* m_target;
	QVariant m_value;
};
class InputConditionDependencyConditionIsGreaterEqual : public InputConditionDependency::Condition
{
public:
	/// Constructor
	InputConditionDependencyConditionIsGreaterEqual(const QDomNode& node, InputConditionContainerSet* cs, InputConditionDependency* dep) :
		InputConditionDependency::Condition(dep)
	{
		QDomElement elem = node.toElement();
		std::string targName = iRIC::toStr(elem.attribute("target"));
		QString valueStr = elem.attribute("value");
		m_target = cs->container(targName);
		m_value = QVariant(valueStr);
		// signal/slot connection
		QObject::connect(m_target, SIGNAL(valueChanged()), dep, SLOT(check()));
	}
	/// Destructor
	~InputConditionDependencyConditionIsGreaterEqual() {}
	bool match() {
		QVariant currentValue = m_target->variantValue();
		return currentValue.toDouble() >= m_value.toDouble();
	}
private:
	InputConditionContainer* m_target;
	QVariant m_value;
};

class InputConditionDependencyConditionIsGreaterThan : public InputConditionDependency::Condition
{
public:
	/// Constructor
	InputConditionDependencyConditionIsGreaterThan(const QDomNode& node, InputConditionContainerSet* cs, InputConditionDependency* dep) :
		InputConditionDependency::Condition(dep)
	{
		QDomElement elem = node.toElement();
		std::string targName = iRIC::toStr(elem.attribute("target"));
		QString valueStr = elem.attribute("value");
		m_target = cs->container(targName);
		m_value = QVariant(valueStr);
		// signal/slot connection
		QObject::connect(m_target, SIGNAL(valueChanged()), dep, SLOT(check()));
	}
	/// Destructor
	~InputConditionDependencyConditionIsGreaterThan() {}
	bool match() {
		QVariant currentValue = m_target->variantValue();
		return currentValue.toDouble() > m_value.toDouble();
	}
private:
	InputConditionContainer* m_target;
	QVariant m_value;
};

class InputConditionDependencyConditionIsLessEqual : public InputConditionDependency::Condition
{
public:
	/// Constructor
	InputConditionDependencyConditionIsLessEqual(const QDomNode& node, InputConditionContainerSet* cs, InputConditionDependency* dep) :
		InputConditionDependency::Condition(dep)
	{
		QDomElement elem = node.toElement();
		std::string targName = iRIC::toStr(elem.attribute("target"));
		QString valueStr = elem.attribute("value");
		m_target = cs->container(targName);
		m_value = QVariant(valueStr);
		// signal/slot connection
		QObject::connect(m_target, SIGNAL(valueChanged()), dep, SLOT(check()));
	}
	/// Destructor
	~InputConditionDependencyConditionIsLessEqual() {}
	bool match() {
		QVariant currentValue = m_target->variantValue();
		return currentValue.toDouble() <= m_value.toDouble();
	}
private:
	InputConditionContainer* m_target;
	QVariant m_value;
};

class InputConditionDependencyConditionIsLessThan : public InputConditionDependency::Condition
{
public:
	/// Constructor
	InputConditionDependencyConditionIsLessThan(const QDomNode& node, InputConditionContainerSet* cs, InputConditionDependency* dep) :
		InputConditionDependency::Condition(dep)
	{
		QDomElement elem = node.toElement();
		std::string targName = iRIC::toStr(elem.attribute("target"));
		QString valueStr = elem.attribute("value");
		m_target = cs->container(targName);
		m_value = QVariant(valueStr);
		// signal/slot connection
		QObject::connect(m_target, SIGNAL(valueChanged()), dep, SLOT(check()));
	}
	/// Destructor
	~InputConditionDependencyConditionIsLessThan() {}
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
		ret = new InputConditionDependencyConditionAlways(node, cs, d);
	} else if (type == "isEqual") {
		ret = new InputConditionDependencyConditionIsEqual(node, cs, d);
	} else if (type == "isGreaterEqual") {
		ret = new InputConditionDependencyConditionIsGreaterEqual(node, cs, d);
	} else if (type == "isGreaterThan") {
		ret = new InputConditionDependencyConditionIsGreaterThan(node, cs, d);
	} else if (type == "isLessEqual") {
		ret = new InputConditionDependencyConditionIsLessEqual(node, cs, d);
	} else if (type == "isLessThan") {
		ret = new InputConditionDependencyConditionIsLessThan(node, cs, d);
	} else if (type == "and") {
		ret = new InputConditionDependencyConditionAnd(node, cs, d);
	} else if (type == "or") {
		ret = new InputConditionDependencyConditionOr(node, cs, d);
	} else if (type == "not") {
		ret = new InputConditionDependencyConditionNot(node, cs, d);
	} else {
		throw ErrorMessage(QString("Unknown condition type %1").arg(type));
	}
	return ret;
}
