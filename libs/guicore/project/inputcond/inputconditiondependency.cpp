#include "inputconditioncontainerset.h"
#include "inputconditiondependency.h"
#include "private/inputconditiondependencyconditionalways.h"
#include "private/inputconditiondependencyconditionand.h"
#include "private/inputconditiondependencyconditionisequal.h"
#include "private/inputconditiondependencyconditionisgreaterequal.h"
#include "private/inputconditiondependencyconditionisgreaterthan.h"
#include "private/inputconditiondependencyconditionislessequal.h"
#include "private/inputconditiondependencyconditionislessthan.h"
#include "private/inputconditiondependencyconditionnot.h"
#include "private/inputconditiondependencyconditionor.h"
#include "private/inputconditionwidgetinteger.h"
#include "private/inputconditionwidgetintegeroption.h"
#include "private/inputconditionwidgetreal.h"
#include "private/inputconditionwidgetrealoption.h"
#include "private/inputconditionwidgetstring.h"

#include <misc/errormessage.h>
#include <misc/stringtool.h>

#include <QList>
#include <QSpinBox>


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

InputConditionDependency::ActionSetValue::ActionSetValue(InputConditionWidget* w, QString value) :
	Action {w},
	m_value {value}
{}

InputConditionDependency::ActionSetValue::~ActionSetValue()
{}

void InputConditionDependency::ActionSetValue::positiveAction()
{
	if (auto widget = dynamic_cast<InputConditionWidgetReal*> (m_target)) {
		widget->m_container->setValue(m_value.toDouble());
	} else if (auto widget = dynamic_cast<InputConditionWidgetInteger*> (m_target)) {
		widget->m_spinBox->setValue(m_value.toInt());
	} else if (auto widget = dynamic_cast<InputConditionWidgetIntegerOption*> (m_target)) {
		widget->setValue(m_value.toInt());
	} else if (auto widget = dynamic_cast<InputConditionWidgetString*> (m_target)) {
		widget->m_container->setValue(m_value.toString());
	} else if (auto widget = dynamic_cast<InputConditionWidgetRealOption*> (m_target)) {
		widget->setValue(m_value.toDouble());
	} else {
		QMessageBox::warning(m_target, "Warning", tr("Unhandled widget in ActionSetValue"));
	}
}

void InputConditionDependency::ActionSetValue::negativeAction()
{
}

InputConditionDependency::Condition::~Condition()
{}

// Definitions of classes those inherit InputConditionDependency::Action

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

InputConditionDependency::Action* InputConditionDependency::buildAction(
	const QDomNode& node,
	InputConditionContainerSet* cs,
	InputConditionWidgetSet* ws,
	InputConditionWidget* w
)
{
	Q_UNUSED(cs);
	Q_UNUSED(ws);

	InputConditionDependency::Action* ret = nullptr;
	QString type = node.toElement().attribute("type");
	if (type == "enable") {
		ret = new InputConditionDependency::ActionEnable(w);
	} else if (type == "setValue") {
		QString value = node.toElement().attribute("value");
		ret = new InputConditionDependency::ActionSetValue(w, value);
	} else {
		throw ErrorMessage(QString("Unknown action type %1").arg(type));
	}
	return ret;
}

QList<InputConditionDependency::Action*> InputConditionDependency::buildActions(
	const QDomNode& condNode,
	InputConditionContainerSet* cs,
	InputConditionWidgetSet* ws,
	InputConditionWidget* w
)
{
	QList<Action*> ret;

	QDomNode parent = condNode.parentNode();

	QDomElement e = parent.firstChildElement(QString("Action"));
	for (; ! e.isNull(); e = e.nextSiblingElement(QString("Action"))) {
		auto a = InputConditionDependency::buildAction(e, cs, ws, w);
		ret.push_back(a);
	}

	// if no actions exist add default enable
	if (ret.isEmpty()) ret.push_back(new InputConditionDependency::ActionEnable(w));

	return ret;
}
