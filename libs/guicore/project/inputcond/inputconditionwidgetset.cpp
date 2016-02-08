#include "../../solverdef/solverdefinitiontranslator.h"
#include "inputconditioncontainerset.h"
#include "inputconditiondependency.h"
#include "inputconditionwidgetfilename.h"
#include "inputconditionwidgetfoldername.h"
#include "inputconditionwidgetfunctional.h"
#include "inputconditionwidgetinteger.h"
#include "inputconditionwidgetintegeroption.h"
#include "inputconditionwidgetreal.h"
#include "inputconditionwidgetrealoption.h"
#include "inputconditionwidgetset.h"
#include "inputconditionwidgetstring.h"

#include <misc/errormessage.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QDomElement>
#include <QDomNode>
#include <QDomNodeList>

InputConditionWidgetSet::InputConditionWidgetSet()
{}

InputConditionWidgetSet::~InputConditionWidgetSet()
{
	clear();
}

void InputConditionWidgetSet::setup(const QDomNode& condNode, InputConditionContainerSet& cset, const SolverDefinitionTranslator& t, bool forBC)
{
	// build widgets first.
	buildWidgets(condNode, cset, t, forBC);
	// build dependencies between the widgets.
	buildDeps(condNode, cset, forBC);
}

InputConditionWidget* InputConditionWidgetSet::widget(const std::string& name) const
{
	return m_widgets.value(name);
}

void InputConditionWidgetSet::clear()
{
	for (auto it = m_widgets.begin(); it != m_widgets.end(); ++it) {
		delete(*it);
	}
	m_widgets.clear();
}

void InputConditionWidgetSet::buildWidgets(const QDomNode& condNode, InputConditionContainerSet& cset, const SolverDefinitionTranslator& t, bool forBC)
{
	if (forBC) {
		buildWidgetsCustom(condNode, cset, t);
	} else {
		QDomNodeList pages = condNode.childNodes();
		for (int i = 0; i < pages.length(); ++i) {
			QDomNode page = pages.item(i);
			buildWidgetsCustom(page, cset, t);
		}
	}
}

void InputConditionWidgetSet::buildWidgetsSimple(const QDomNode& contNode, InputConditionContainerSet& cset, const SolverDefinitionTranslator& t)
{
	// access items node
	QDomNode itemsNode = iRIC::getChildNode(contNode, "Items");
	if (itemsNode.isNull()) {return;}
	QDomNodeList items = itemsNode.childNodes();
	for (int i = 0; i < items.length(); ++i) {
		QDomNode itemNode = items.item(i);
		buildWidget(itemNode, cset, t);
	}
}

void InputConditionWidgetSet::buildWidgetsCustom(const QDomNode& contNode, InputConditionContainerSet& cset, const SolverDefinitionTranslator& t)
{
	// get Content Node.
	buildWidgetsCustomRec(contNode, cset, t);
}

void InputConditionWidgetSet::buildWidgetsCustomRec(const QDomNode& node, InputConditionContainerSet& cset, const SolverDefinitionTranslator& t)
{
	QDomNodeList children = node.childNodes();
	for (int i = 0; i < children.length(); ++i) {
		QDomNode c = children.item(i);
		if (c.nodeType() == QDomNode::ElementNode) {
			if (c.nodeName() == "Item") {
				// build item.
				buildWidget(c, cset, t);
			} else {
				// search item recursively.
				buildWidgetsCustomRec(c, cset, t);
			}
		}
	}
}

void InputConditionWidgetSet::buildWidget(QDomNode& itemNode, InputConditionContainerSet& cset, const SolverDefinitionTranslator& t)
{
	// get the name;
	QDomElement itemElem = itemNode.toElement();
	std::string parameterName = iRIC::toStr(itemElem.attribute("name"));
	// get the definition node;
	QDomNode defNode = iRIC::getChildNode(itemNode, "Definition");
	if (defNode.isNull()) {
		// this item doesn't contain "Definition" Node!".
		throw(ErrorMessage("Definition node does not exist!"));
	}
	QDomElement defElem = defNode.toElement();
	QString type = defElem.attribute("conditionType", "");
	// setup container depending on the type
	InputConditionWidget* widget;
	if (type == "functional") {
		widget = new InputConditionWidgetFunctional(defNode, t, &(cset.functional(parameterName)));
		m_widgets.insert(parameterName, widget);
	} else if (type == "constant" || type == "") {
		QString valuetype = defElem.attribute("valueType");
		if (valuetype == "integer") {
			if (InputConditionWidget::hasEnums(defNode)) {
				widget = new InputConditionWidgetIntegerOption(defNode, t, &(cset.integer(parameterName)));
			} else {
				widget = new InputConditionWidgetInteger(defNode, t, &(cset.integer(parameterName)), cset);
			}
		} else if (valuetype == "real") {
			if (InputConditionWidget::hasEnums(defNode)) {
				widget = new InputConditionWidgetRealOption(defNode, t, &(cset.real(parameterName)));
			} else {
				widget = new InputConditionWidgetReal(defNode, t, &(cset.real(parameterName)));
			}
		} else if (valuetype == "string") {
			widget = new InputConditionWidgetString(defNode, t, &(cset.string(parameterName)));
		} else if (valuetype == "filename" || valuetype == "filename_all") {
			widget = new InputConditionWidgetFilename(defNode, t, &(cset.string(parameterName)));
		} else if (valuetype == "foldername") {
			widget = new InputConditionWidgetFoldername(defNode, t, &(cset.string(parameterName)));
		} else if (valuetype == "functional") {
			widget = new InputConditionWidgetFunctional(defNode, t, &(cset.functional(parameterName)));
		} else {
			throw(ErrorMessage("Wrong conditionType is set."));
		}
		addTooltip(widget, defNode, t);
		m_widgets.insert(parameterName, widget);
	} else {
		throw(ErrorMessage("Wrong conditionType is set."));
	}
}

void InputConditionWidgetSet::buildDeps(const QDomNode& ccNode, InputConditionContainerSet& cset, bool forBC)
{
	if (forBC) {
		buildDepsCustom(ccNode, cset);
	} else {
		QDomNodeList pages = ccNode.childNodes();
		for (int i = 0; i < pages.length(); ++i) {
			QDomNode page = pages.item(i);
			buildDepsCustom(page, cset);
		}
	}
}

void InputConditionWidgetSet::buildDepsSimple(const QDomNode& tabNode, InputConditionContainerSet& cset)
{
	// access content/items/
	QDomNode cont = tabNode.firstChild();
	while (! cont.isNull() && cont.nodeName() != QString("Content")) {
		cont = cont.nextSibling();
	}
	if (cont.isNull()) {return;}
	QDomNode itemsNode = cont.firstChild();
	while (! itemsNode.isNull() && itemsNode.nodeName() != QString("Items")) {
		itemsNode = itemsNode.nextSibling();
	}
	if (itemsNode.isNull()) {return;}

	// Now, I've got the itemsNode!
	QDomNodeList items = itemsNode.childNodes();
	for (int j = 0; j < items.length(); ++j) {
		QDomNode itemNode = items.item(j);
		buildDepsItem(itemNode, cset);
	}
}
void InputConditionWidgetSet::buildDepsCustom(const QDomNode& pageNode, InputConditionContainerSet& cset)
{
	buildDepsCustomRec(pageNode, cset);
}
void InputConditionWidgetSet::buildDepsCustomRec(const QDomNode& node, InputConditionContainerSet& cset)
{
	QDomNodeList children = node.childNodes();
	for (int i = 0; i < children.length(); ++i) {
		QDomNode c = children.item(i);
		if (c.nodeType() == QDomNode::ElementNode) {
			if (c.nodeName() == "Item") {
				// build dependency.
				buildDepsItem(c, cset);
			} else {
				// search item recursively.
				buildDepsCustomRec(c, cset);
			}
		}
	}
}
void InputConditionWidgetSet::buildDepsItem(const QDomNode& itemNode, InputConditionContainerSet& cset)
{
	std::string parameterName = iRIC::toStr(itemNode.toElement().attribute("name"));
	// get the definition node;
	QDomNode defNode = iRIC::getChildNode(itemNode, "Definition");
	if (defNode.isNull()) {
		// this item doesn't contain "Definition" Node!".
		throw(ErrorMessage("Definition node does not exist!"));
	}
	InputConditionWidget* w = this->widget(parameterName);

	// Search for "Dependency" node.
	QDomNode depNode = iRIC::getChildNode(defNode, "Dependency");
	if (! depNode.isNull()) {
		QDomNode condNode = iRIC::getChildNode(depNode, "Condition");
		if (condNode.isNull()) {return;}
		buildDep(condNode, cset, w);
	} else {
		// support "Condition" node just under definition node.
		QDomNode condNode = iRIC::getChildNode(defNode, "Condition");
		if (condNode.isNull()) {return;}
		buildDep(condNode, cset, w);
	}
}

void InputConditionWidgetSet::buildDep(const QDomNode& condNode, InputConditionContainerSet& cset, InputConditionWidget* w)
{
	// Find condition node
	InputConditionDependency* dep = new InputConditionDependency();
	InputConditionDependency::Condition* cond = InputConditionDependency::buildCondition(condNode, &cset, dep);
	dep->setCondition(cond);
	// Action corresponding to Condition is now enabling only.
	InputConditionDependency::Action* a = new InputConditionDependency::ActionEnable(w);
	dep->addAction(a);
	w->setDependency(dep);
	// Initial Check
	dep->check();
}

void InputConditionWidgetSet::addTooltip(QWidget* widget, QDomNode defNode, const SolverDefinitionTranslator& t)
{
	QDomElement defElem = defNode.toElement();
	QString tooltip = defElem.attribute("tooltips");
	if (tooltip != "") {
		widget->setToolTip(t.translate(tooltip));
	}
}

void InputConditionWidgetSet::disableWidgets()
{
	for (auto it = m_widgets.begin(); it != m_widgets.end(); ++it) {
		InputConditionWidget* w = it.value();
		w->setDisabled(true);
	}
}

void InputConditionWidgetSet::enableWidgets()
{
	for (auto it = m_widgets.begin(); it != m_widgets.end(); ++it) {
		InputConditionWidget* w = it.value();
		w->setEnabled(true);
	}
	for (auto it = m_widgets.begin(); it != m_widgets.end(); ++it) {
		InputConditionWidget* w = it.value();
		InputConditionDependency* dep = w->dependency();
		if (dep != nullptr) {
			dep->check();
		}
	}
}
