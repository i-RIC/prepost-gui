#include "cgnsfileinputconditionwidgetset.h"

#include "cgnsfileinputconditionwidgetinteger.h"
#include "cgnsfileinputconditionwidgetintegeroption.h"
#include "cgnsfileinputconditionwidgetreal.h"
#include "cgnsfileinputconditionwidgetrealoption.h"
#include "cgnsfileinputconditionwidgetstring.h"
#include "cgnsfileinputconditionwidgetfilename.h"
#include "cgnsfileinputconditionwidgetfoldername.h"
#include "cgnsfileinputconditionwidgetfunctional.h"

#include "cgnsfileinputconditioncontainerset.h"
#include "cgnsfileinputconditiondependency.h"
#include <misc/xmlsupport.h>
#include "../../solverdef/solverdefinitiontranslator.h"

#include <misc/errormessage.h>

#include <QDomNode>
#include <QDomElement>
#include <QDomNodeList>

CgnsFileInputConditionWidgetSet::~CgnsFileInputConditionWidgetSet()
{
	clear();
}

void CgnsFileInputConditionWidgetSet::setup(const QDomNode& condNode, CgnsFileInputConditionContainerSet& cset, const SolverDefinitionTranslator& t, bool forBC){
	// build widgets first.
	buildWidgets(condNode, cset, t, forBC);
	// build dependencies between the widgets.
	buildDeps(condNode, cset, forBC);
}

void CgnsFileInputConditionWidgetSet::clear()
{
	QMap<QString, CgnsFileInputConditionWidget*>::iterator it;
	for (it = m_widgets.begin(); it != m_widgets.end(); ++it){
		delete (*it);
	}
	m_widgets.clear();
}

void CgnsFileInputConditionWidgetSet::buildWidgets(const QDomNode& condNode, CgnsFileInputConditionContainerSet& cset, const SolverDefinitionTranslator& t, bool forBC)
{
	if (forBC){
		buildWidgetsCustom(condNode, cset, t);
	} else {
		QDomNodeList pages = condNode.childNodes();
		for (unsigned int i = 0; i < pages.length(); ++i){
			QDomNode page = pages.item(i);
			buildWidgetsCustom(page, cset, t);
		}
	}
}

void CgnsFileInputConditionWidgetSet::buildWidgetsSimple(const QDomNode& contNode, CgnsFileInputConditionContainerSet& cset, const SolverDefinitionTranslator& t)
{
	// access items node
	QDomNode itemsNode = iRIC::getChildNode(contNode, "Items");
	if (itemsNode.isNull()){return;}
	QDomNodeList items = itemsNode.childNodes();
	for (unsigned int i = 0; i < items.length(); ++i){
		QDomNode itemNode = items.item(i);
		buildWidget(itemNode, cset, t);
	}
}

void CgnsFileInputConditionWidgetSet::buildWidgetsCustom(const QDomNode& contNode, CgnsFileInputConditionContainerSet& cset, const SolverDefinitionTranslator& t){
	// get Content Node.
	buildWidgetsCustomRec(contNode, cset, t);
}

void CgnsFileInputConditionWidgetSet::buildWidgetsCustomRec(const QDomNode& node, CgnsFileInputConditionContainerSet& cset, const SolverDefinitionTranslator& t){
	QDomNodeList children = node.childNodes();
	for (unsigned int i = 0; i < children.length(); ++i){
		QDomNode c = children.item(i);
		if (c.nodeType() == QDomNode::ElementNode){
			if (c.nodeName() == "Item"){
				// build item.
				buildWidget(c, cset, t);
			}else{
				// search item recursively.
				buildWidgetsCustomRec(c, cset, t);
			}
		}
	}
}

void CgnsFileInputConditionWidgetSet::buildWidget(QDomNode& itemNode, CgnsFileInputConditionContainerSet& cset, const SolverDefinitionTranslator& t){
	static QString optstr("option");
	static QString truestr("true");
	// get the name;
	QDomElement itemElem = itemNode.toElement();
	QString parameterName = itemElem.attribute("name");
	// get the definition node;
	QDomNode defNode = iRIC::getChildNode(itemNode, "Definition");
	if (defNode.isNull()){
		// this item doesn't contain "Definition" Node!".
		throw (ErrorMessage("Definition node does not exist!"));
	}
	QDomElement defElem = defNode.toElement();
	QString type = defElem.attribute("conditionType", "");
	// setup container depending on the type
	CgnsFileInputConditionWidget* widget;
	if (type == "functional"){
		widget = new CgnsFileInputConditionWidgetFunctional(defNode, t, &(cset.functional(parameterName)));
		m_widgets.insert(parameterName, widget);
	} else if (type == "constant" || type == ""){
		QString valuetype = defElem.attribute("valueType");
		if (valuetype == "integer"){
			if (CgnsFileInputConditionWidget::hasEnums(defNode)){
				widget = new CgnsFileInputConditionWidgetIntegerOption(defNode, t, &(cset.integer(parameterName)));
			} else {
				widget = new CgnsFileInputConditionWidgetInteger(defNode, t, &(cset.integer(parameterName)), cset);
			}
		} else if (valuetype == "real"){
			if (CgnsFileInputConditionWidget::hasEnums(defNode)){
				widget = new CgnsFileInputConditionWidgetRealOption(defNode, t, &(cset.real(parameterName)));
			} else {
				widget = new CgnsFileInputConditionWidgetReal(defNode, t, &(cset.real(parameterName)));
			}
		} else if (valuetype == "string"){
			widget = new CgnsFileInputConditionWidgetString(defNode, t, &(cset.string(parameterName)));
		} else if (valuetype == "filename" || valuetype == "filename_all"){
			widget = new CgnsFileInputConditionWidgetFilename(defNode, t, &(cset.string(parameterName)));
		} else if (valuetype == "foldername"){
			widget = new CgnsFileInputConditionWidgetFoldername(defNode, t, &(cset.string(parameterName)));
		} else if (valuetype == "functional"){
			widget = new CgnsFileInputConditionWidgetFunctional(defNode, t, &(cset.functional(parameterName)));
		} else {
			throw (ErrorMessage("Wrong conditionType is set."));
		}
		addTooltip(widget, defNode, t);
		m_widgets.insert(parameterName, widget);
	}else{
		throw (ErrorMessage("Wrong conditionType is set."));
	}
}

void CgnsFileInputConditionWidgetSet::buildDeps(const QDomNode& ccNode, CgnsFileInputConditionContainerSet& cset, bool forBC){
	if (forBC){
		buildDepsCustom(ccNode, cset);
	} else {
		QDomNodeList pages = ccNode.childNodes();
		for (unsigned int i = 0; i < pages.length(); ++i){
			QDomNode page = pages.item(i);
			buildDepsCustom(page, cset);
		}
	}
}

void CgnsFileInputConditionWidgetSet::buildDepsSimple(const QDomNode& tabNode, CgnsFileInputConditionContainerSet& cset){
	// access content/items/
	QDomNode cont = tabNode.firstChild();
	while (! cont.isNull() && cont.nodeName() != QString("Content")){
		cont = cont.nextSibling();
	}
	if (cont.isNull()){return;}
	QDomNode itemsNode = cont.firstChild();
	while (! itemsNode.isNull() && itemsNode.nodeName() != QString("Items")){
		itemsNode = itemsNode.nextSibling();
	}
	if (itemsNode.isNull()){return;}

	// Now, I've got the itemsNode!
	QDomNodeList items = itemsNode.childNodes();
	for (unsigned int j = 0; j < items.length(); ++j){
		QDomNode itemNode = items.item(j);
		buildDepsItem(itemNode, cset);
	}
}
void CgnsFileInputConditionWidgetSet::buildDepsCustom(const QDomNode& pageNode, CgnsFileInputConditionContainerSet& cset){
	buildDepsCustomRec(pageNode, cset);
}
void CgnsFileInputConditionWidgetSet::buildDepsCustomRec(const QDomNode& node, CgnsFileInputConditionContainerSet& cset){
	QDomNodeList children = node.childNodes();
	for (unsigned int i = 0; i < children.length(); ++i){
		QDomNode c = children.item(i);
		if (c.nodeType() == QDomNode::ElementNode){
			if (c.nodeName() == "Item"){
				// build dependency.
				buildDepsItem(c, cset);
			}else{
				// search item recursively.
				buildDepsCustomRec(c, cset);
			}
		}
	}
}
void CgnsFileInputConditionWidgetSet::buildDepsItem(const QDomNode& itemNode, CgnsFileInputConditionContainerSet& cset){
	// get the name;
	QString parameterName = itemNode.toElement().attribute("name");
	// get the definition node;
	QDomNode defNode = iRIC::getChildNode(itemNode, "Definition");
	if (defNode.isNull()){
		// this item doesn't contain "Definition" Node!".
		throw (ErrorMessage("Definition node does not exist!"));
	}
	CgnsFileInputConditionWidget* w = this->widget(parameterName);

	// Search for "Dependency" node.
	QDomNode depNode = iRIC::getChildNode(defNode, "Dependency");
	if (! depNode.isNull()){
		QDomNode condNode = iRIC::getChildNode(depNode, "Condition");
		if (condNode.isNull()){return;}
		buildDep(condNode, cset, w);
	} else {
		// support "Condition" node just under definition node.
		QDomNode condNode = iRIC::getChildNode(defNode, "Condition");
		if (condNode.isNull()){return;}
		buildDep(condNode, cset, w);
	}
}

void CgnsFileInputConditionWidgetSet::buildDep(const QDomNode& condNode, CgnsFileInputConditionContainerSet& cset, CgnsFileInputConditionWidget* w){
	// Find condition node
	CgnsFileInputConditionDependency* dep = new CgnsFileInputConditionDependency();
	CgnsFileInputConditionDependency::Condition* cond = CgnsFileInputConditionDependency::buildCondition(condNode, &cset, dep);
	dep->setCondition(cond);
	// Action corresponding to Condition is now enabling only.
	CgnsFileInputConditionDependency::Action* a = new CgnsFileInputConditionDependency::ActionEnable(w);
	dep->addAction(a);
	w->setDependency(dep);
	// Initial Check
	dep->check();
}

void CgnsFileInputConditionWidgetSet::addTooltip(QWidget* widget, QDomNode defNode, const SolverDefinitionTranslator& t){
	QDomElement defElem = defNode.toElement();
	QString tooltip = defElem.attribute("tooltips");
	if (tooltip != ""){
		widget->setToolTip(t.translate(tooltip));
	}
}

void CgnsFileInputConditionWidgetSet::disableWidgets()
{
	QMap<QString, CgnsFileInputConditionWidget*>::iterator it;
	for (it = m_widgets.begin(); it != m_widgets.end(); ++it){
		CgnsFileInputConditionWidget* w = it.value();
		w->setDisabled(true);
	}
}

void CgnsFileInputConditionWidgetSet::enableWidgets()
{
	QMap<QString, CgnsFileInputConditionWidget*>::iterator it;
	for (it = m_widgets.begin(); it != m_widgets.end(); ++it){
		CgnsFileInputConditionWidget* w = it.value();
		w->setEnabled(true);
	}
	for (it = m_widgets.begin(); it != m_widgets.end(); ++it){
		CgnsFileInputConditionWidget* w = it.value();
		CgnsFileInputConditionDependency* dep = w->dependency();
		if (dep != 0){
			dep->check();
		}
	}
}
