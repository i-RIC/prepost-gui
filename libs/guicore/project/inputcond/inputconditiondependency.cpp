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
