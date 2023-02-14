#include "../../solverdef/solverdefinition.h"
#include "../../solverdef/solverdefinitionabstract.h"
#include "../../solverdef/solverdefinitiontranslator.h"
#include "inputconditioncontainerset.h"
#include "inputconditiondependency.h"
#include "inputconditionwidgetfilename.h"
#include "inputconditionwidgetset.h"

#include "private/inputconditioncgnsfile.h"
#include "private/inputconditiondependencychecksubcaptions.h"
#include "private/inputconditiondependencychecksubenumerations.h"
#include "private/inputconditiondependencychecksubimages.h"
#include "private/inputconditiondependencysetsubcaptionaction.h"
#include "private/inputconditiondependencysetsubenumerationsaction.h"
#include "private/inputconditiondependencysetsubimageaction.h"
#include "private/inputconditionwidgetcgnsbaseiterativeintegerresult.h"
#include "private/inputconditionwidgetcgnsbaseiterativerealresult.h"
#include "private/inputconditionwidgetcgnsfile.h"
#include "private/inputconditionwidgetcgnsgridcellintegerresult.h"
#include "private/inputconditionwidgetcgnsgridcellrealresult.h"
#include "private/inputconditionwidgetcgnsgridedgeiintegerresult.h"
#include "private/inputconditionwidgetcgnsgridedgeirealresult.h"
#include "private/inputconditionwidgetcgnsgridedgejintegerresult.h"
#include "private/inputconditionwidgetcgnsgridedgejrealresult.h"
#include "private/inputconditionwidgetcgnsgridnodeintegerresult.h"
#include "private/inputconditionwidgetcgnsgridnoderealresult.h"
#include "private/inputconditionwidgetcheckbox.h"
#include "private/inputconditionwidgetfoldername.h"
#include "private/inputconditionwidgetfunctional.h"
#include "private/inputconditionwidgetimage.h"
#include "private/inputconditionwidgetinteger.h"
#include "private/inputconditionwidgetintegeroption.h"
#include "private/inputconditionwidgetlabel.h"
#include "private/inputconditionwidgetreal.h"
#include "private/inputconditionwidgetrealoption.h"
#include "private/inputconditionwidgetstring.h"

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

void InputConditionWidgetSet::setup(const QDomNode& condNode, InputConditionContainerSet& cset, const SolverDefinition& def, const SolverDefinitionTranslator& t, bool forBC)
{
	// build widgets first.
	buildWidgets(condNode, cset, def, t, forBC);
	// build dependencies between the widgets.
	buildDeps(condNode, cset, forBC);
}

InputConditionWidget* InputConditionWidgetSet::widget(const std::string& name) const
{
	return m_widgets.value(name);
}

bool InputConditionWidgetSet::checkImportSourceUpdate()
{
	bool ret = false;
	for (auto w : m_widgets) {
		ret = ret || w->checkImportSourceUpdate();
	}
	return ret;
}

void InputConditionWidgetSet::clear()
{
	for (auto w : m_widgets) {
		delete(w);
	}
	m_widgets.clear();
}

void InputConditionWidgetSet::buildWidgets(const QDomNode& condNode, InputConditionContainerSet& cset, const SolverDefinition& def, const SolverDefinitionTranslator& t, bool forBC)
{
	if (forBC) {
		buildWidgetsCustom(condNode, cset, def, t);
	} else {
		QDomNodeList pages = condNode.childNodes();
		for (int i = 0; i < pages.length(); ++i) {
			QDomNode page = pages.item(i);
			buildWidgetsCustom(page, cset, def, t);
		}
	}
}

void InputConditionWidgetSet::buildWidgetsCustom(const QDomNode& contNode, InputConditionContainerSet& cset, const SolverDefinition& def, const SolverDefinitionTranslator& t)
{
	// get Content Node.
	buildWidgetsCustomRec(contNode, cset, def, t);
}

void InputConditionWidgetSet::buildWidgetsCustomRec(const QDomNode& node, InputConditionContainerSet& cset, const SolverDefinition& def, const SolverDefinitionTranslator& t)
{
	QDomNodeList children = node.childNodes();
	for (int i = 0; i < children.length(); ++i) {
		QDomNode c = children.item(i);
		if (c.nodeType() == QDomNode::ElementNode) {
			if (c.nodeName() == "Item") {
				// build item.
				buildWidget(c, cset, def, t);
				if (c.toElement().attributes().contains("caption")) {
					buildLabel(c, t);
				}
			} else if (c.nodeName() == "Image"){
				// build image.
				buildImage(c, def, t);
			} else if (c.nodeName() == "Label"){
				buildLabel(c, t);
			} else {
				// search item recursively.
				buildWidgetsCustomRec(c, cset, def, t);
			}
		}
	}
}

void InputConditionWidgetSet::buildWidget(QDomNode& itemNode, InputConditionContainerSet& cset, const SolverDefinition& def, const SolverDefinitionTranslator& t)
{
	// get the name;
	QDomElement itemElem = itemNode.toElement();
	std::string parameterName = iRIC::toStr(itemElem.attribute("name"));
	QString parameterCaption = itemElem.attribute("caption");
	// get the definition node;
	QDomNode defNode = iRIC::getChildNode(itemNode, "Definition");
	if (defNode.isNull()) {
		// this item doesn't contain "Definition" Node!".
		throw(ErrorMessage("Definition node does not exist!"));
	}
	QDomElement defElem = defNode.toElement();
	QString type = defElem.attribute("conditionType", "");
	// setup container depending on the type
	InputConditionWidget* widget = nullptr;
	if (type == "functional") {
		auto w = new InputConditionWidgetFunctional(defNode, t, &(cset.functional(parameterName)));
		if (! parameterCaption.isNull()) {
			w->setCaption(parameterCaption);
		}
		widget = w;
		m_widgets.insert(parameterName, widget);
	} else if (type == "constant" || type == "") {
		QString valuetype = defElem.attribute("valueType");
		std::string cgnsFile = iRIC::toStr(defElem.attribute("cgnsFile"));
		if (valuetype == "integer") {
			if (defElem.attribute("checkBox", "false") == "true") {
				widget = new InputConditionWidgetCheckbox(defNode, t, &(cset.integer(parameterName)));
			} else if (InputConditionWidget::hasEnums(defNode)) {
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
			widget = new InputConditionWidgetString(defNode, t, &(cset.string(parameterName)), def.abstract().absoluteFolderName());
		} else if (valuetype == "filename" || valuetype == "filename_all") {
			widget = new InputConditionWidgetFilename(defNode, t, &(cset.string(parameterName)));
		} else if (valuetype == "foldername") {
			widget = new InputConditionWidgetFoldername(defNode, t, &(cset.string(parameterName)));
		} else if (valuetype == "functional") {
			auto w = new InputConditionWidgetFunctional(defNode, t, &(cset.functional(parameterName)));
			if (! parameterCaption.isNull()) {
				w->setCaption(parameterCaption);
			}
			widget = w;
		} else if (valuetype == "cgns_filename") {
			widget = new InputConditionWidgetCgnsFile(defNode, t, &(cset.string(parameterName)), getCgnsFile(parameterName));
		} else if (valuetype == "result_gridNodeInteger") {
			if (cgnsFile.size() == 0) {throw (ErrorMessage(QString("cgnsFile attribute is not set for %1").arg(parameterName.c_str())));}
			widget = new InputConditionWidgetCgnsGridNodeIntegerResult(defNode, t, &(cset.string(parameterName)), getCgnsFile(cgnsFile));
		} else if (valuetype == "result_gridNodeReal") {
			if (cgnsFile.size() == 0) {throw (ErrorMessage(QString("cgnsFile attribute is not set for %1").arg(parameterName.c_str())));}
			widget = new InputConditionWidgetCgnsGridNodeRealResult(defNode, t, &(cset.string(parameterName)), getCgnsFile(cgnsFile));
		} else if (valuetype == "result_gridCellInteger") {
			if (cgnsFile.size() == 0) {throw (ErrorMessage(QString("cgnsFile attribute is not set for %1").arg(parameterName.c_str())));}
			widget = new InputConditionWidgetCgnsGridCellIntegerResult(defNode, t, &(cset.string(parameterName)), getCgnsFile(cgnsFile));
		} else if (valuetype == "result_gridCellReal") {
			if (cgnsFile.size() == 0) {throw (ErrorMessage(QString("cgnsFile attribute is not set for %1").arg(parameterName.c_str())));}
			widget = new InputConditionWidgetCgnsGridCellRealResult(defNode, t, &(cset.string(parameterName)), getCgnsFile(cgnsFile));
		} else if (valuetype == "result_gridEdgeIInteger") {
			if (cgnsFile.size() == 0) {throw (ErrorMessage(QString("cgnsFile attribute is not set for %1").arg(parameterName.c_str())));}
			widget = new InputConditionWidgetCgnsGridEdgeIIntegerResult(defNode, t, &(cset.string(parameterName)), getCgnsFile(cgnsFile));
		} else if (valuetype == "result_gridEdgeIReal") {
			if (cgnsFile.size() == 0) {throw (ErrorMessage(QString("cgnsFile attribute is not set for %1").arg(parameterName.c_str())));}
			widget = new InputConditionWidgetCgnsGridEdgeIRealResult(defNode, t, &(cset.string(parameterName)), getCgnsFile(cgnsFile));
		} else if (valuetype == "result_gridEdgeJInteger") {
			if (cgnsFile.size() == 0) {throw (ErrorMessage(QString("cgnsFile attribute is not set for %1").arg(parameterName.c_str())));}
			widget = new InputConditionWidgetCgnsGridEdgeJIntegerResult(defNode, t, &(cset.string(parameterName)), getCgnsFile(cgnsFile));
		} else if (valuetype == "result_gridEdgeJReal") {
			if (cgnsFile.size() == 0) {throw (ErrorMessage(QString("cgnsFile attribute is not set for %1").arg(parameterName.c_str())));}
			widget = new InputConditionWidgetCgnsGridEdgeJRealResult(defNode, t, &(cset.string(parameterName)), getCgnsFile(cgnsFile));
		} else if (valuetype == "result_baseIterativeInteger") {
			if (cgnsFile.size() == 0) {throw (ErrorMessage(QString("cgnsFile attribute is not set for %1").arg(parameterName.c_str())));}
			widget = new InputConditionWidgetCgnsBaseIterativeIntegerResult(defNode, t, &(cset.string(parameterName)), getCgnsFile(cgnsFile));
		} else if (valuetype == "result_baseIterativeReal") {
			if (cgnsFile.size() == 0) {throw (ErrorMessage(QString("cgnsFile attribute is not set for %1").arg(parameterName.c_str())));}
			widget = new InputConditionWidgetCgnsBaseIterativeRealResult(defNode, t, &(cset.string(parameterName)), getCgnsFile(cgnsFile));
		} else {
			throw(ErrorMessage("Wrong conditionType is set."));
		}
		addTooltip(widget, itemNode, t);
		m_widgets.insert(parameterName, widget);
	} else {
		throw(ErrorMessage("Wrong conditionType is set."));
	}
}

void InputConditionWidgetSet::buildLabel(QDomNode& itemNode, const SolverDefinitionTranslator& t)
{
	auto itemElem = itemNode.toElement();
	auto name = iRIC::toStr(labelName(itemNode));
	auto widget = new InputConditionWidgetLabel(itemElem, t);
	m_widgets.insert(name, widget);
}

void InputConditionWidgetSet::buildImage(QDomNode& itemNode, const SolverDefinition& def, const SolverDefinitionTranslator& t)
{
	auto itemElem = itemNode.toElement();
	auto src = iRIC::toStr(itemElem.attribute("src"));
	auto widget = new InputConditionWidgetImage(itemElem, t, def.folder());
	m_widgets.insert(src, widget);
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
	auto children = node.childNodes();
	for (int i = 0; i < children.length(); ++i) {
		auto c = children.item(i);
		if (c.nodeType() == QDomNode::ElementNode) {
			if (c.nodeName() == "Item") {
				// build dependency.
				buildDepsItem(c, cset);
				if (c.toElement().attributes().contains("caption")) {
					buildDepsLabel(c, cset);
				}
				QDomNode defNode = iRIC::getChildNode(c, "Definition");
				if (InputConditionWidget::hasEnums(defNode)) {
					buildDepsOption(c, cset);
				}
			} else if (c.nodeName() == "Image") {
				// build dependency for image (subimages)
				buildDepsImage(c, cset);
			} else if (c.nodeName() == "Label") {
				buildDepsLabel(c, cset);
			} else {
				// search item recursively.
				buildDepsCustomRec(c, cset);
			}
		}
	}
}
void InputConditionWidgetSet::buildDepsItem(const QDomNode& itemNode, InputConditionContainerSet& cset)
{
	auto parameterName = iRIC::toStr(itemNode.toElement().attribute("name"));
	// get the definition node;
	QDomNode defNode = iRIC::getChildNode(itemNode, "Definition");
	if (defNode.isNull()) {
		// this item doesn't contain "Definition" Node!".
		throw(ErrorMessage("Definition node does not exist!"));
	}
	auto w = this->widget(parameterName);

	// Search for "Dependency" node.
	auto depNode = iRIC::getChildNode(defNode, "Dependency");
	if (! depNode.isNull()) {
		for (; ! depNode.isNull(); depNode = depNode.nextSiblingElement("Dependency")) {
			auto condNode = iRIC::getChildNode(depNode, "Condition");
			if (condNode.isNull()) {continue;}
			buildDep(condNode, cset, w);
		}
	} else {
		// support "Condition" node just under definition node.
		auto condNode = iRIC::getChildNode(defNode, "Condition");
		if (condNode.isNull()) {return;}
		buildDep(condNode, cset, w);
	}
}

void InputConditionWidgetSet::buildDepsOption(const QDomNode& itemNode, InputConditionContainerSet& cset)
{
	QDomElement itemElem = itemNode.toElement();
	std::string parameterName = iRIC::toStr(itemElem.attribute("name"));
	auto combo = dynamic_cast<InputConditionWidgetIntegerOption*> (this->widget(parameterName));

	// get the definition node;
	QDomNode defNode = iRIC::getChildNode(itemNode, "Definition");
	if (defNode.isNull()) return;

	// get the enumerations node
	QDomNode enumsNode = iRIC::getChildNode(defNode, "Enumerations");
	if (enumsNode.isNull()) return;

	size_t index;
	QDomElement subEnumsElem = enumsNode.firstChildElement("SubEnumerations");
	for (index = 0; ! subEnumsElem.isNull(); subEnumsElem = subEnumsElem.nextSiblingElement("SubEnumerations"), ++index) {
		auto condNode = iRIC::getChildNode(subEnumsElem, "Condition");
		auto name = InputConditionWidgetIntegerOption::subEnumerationsName(subEnumsElem, index);
		auto dep = new InputConditionDependency();
		auto cond = InputConditionDependency::buildCondition(condNode, &cset, combo->checkSubEnumerations());
		dep->setCondition(cond);
		auto a = new InputConditionDependencySetSubEnumerationsAction(combo, name);
		dep->addAction(a);
		combo->addDependency(dep);
	}
	combo->checkSubEnumerations()->check();
}

void InputConditionWidgetSet::buildDepsLabel(const QDomNode& labelNode, InputConditionContainerSet& cset)
{
	auto lName = iRIC::toStr(labelName(labelNode));
	auto label = dynamic_cast<InputConditionWidgetLabel*> (this->widget(lName));

	auto children = labelNode.childNodes();
	for (int i = 0; i < children.count(); ++i) {
		auto subCaptionNode = children.at(i);
		if (subCaptionNode.nodeName() != "SubCaption") {
			continue;
		}
		auto condNode = iRIC::getChildNode(subCaptionNode, "Condition");
		auto caption = subCaptionNode.toElement().attribute("caption");
		auto dep = new InputConditionDependency();
		auto cond = InputConditionDependency::buildCondition(condNode, &cset, label->checkSubCaptions());
		dep->setCondition(cond);
		auto a = new InputConditionDependencySetSubCaptionAction(label, caption);
		dep->addAction(a);
		label->addDependency(dep);
	}
	label->checkSubCaptions()->check();
}

void InputConditionWidgetSet::buildDepsImage(const QDomNode& imageNode, InputConditionContainerSet& cset)
{
	auto imageName = iRIC::toStr(imageNode.toElement().attribute("src"));
	auto img = dynamic_cast<InputConditionWidgetImage*> (this->widget(imageName));

	auto children = imageNode.childNodes();
	for (int i = 0; i < children.count(); ++i) {
		auto subImageNode = children.at(i);
		if (subImageNode.nodeName() != "SubImage") {
			continue;
		}
		auto condNode = iRIC::getChildNode(subImageNode, "Condition");
		auto name = subImageNode.toElement().attribute("src");
		auto dep = new InputConditionDependency();
		auto cond = InputConditionDependency::buildCondition(condNode, &cset, img->checkSubImages());
		dep->setCondition(cond);
		auto a = new InputConditionDependencySetSubimageAction(img, name);
		dep->addAction(a);
		img->addDependency(dep);
	}
	img->checkSubImages()->check();
}

void InputConditionWidgetSet::buildDep(const QDomNode& condNode, InputConditionContainerSet& cset, InputConditionWidget* w)
{
	// Find condition node
	auto dep = new InputConditionDependency();
	auto cond = InputConditionDependency::buildCondition(condNode, &cset, dep);
	dep->setCondition(cond);
	// Action corresponding to Condition is now enabling only.
	auto actions = InputConditionDependency::buildActions(condNode, &cset, this, w);
	for (auto a : actions) {
		dep->addAction(a);
	}
	w->addDependency(dep);
	// Initial Check
	dep->check();
}

void InputConditionWidgetSet::addTooltip(InputConditionWidget* widget, QDomNode itemNode, const SolverDefinitionTranslator& t)
{
	auto itemElem = itemNode.toElement();
	auto tooltip = itemElem.attribute("tooltips");
	if (tooltip == "") {return;}

	widget->addTooltip(t.translate(tooltip));
}

void InputConditionWidgetSet::toggleReadOnly(bool readonly)
{
	for (auto it = m_widgets.begin(); it != m_widgets.end(); ++it) {
		InputConditionWidget* w = it.value();
		w->toggleReadOnly(readonly);
	}

	if (readonly) {return;}

	for (auto it = m_widgets.begin(); it != m_widgets.end(); ++it) {
		InputConditionWidget* w = it.value();
		for (auto dep : w->dependencies()) {
			dep->check();
		}
	}
}

QString InputConditionWidgetSet::labelName(const QDomNode& itemNode)
{
	auto itemElem = itemNode.toElement();
	if (itemElem.attributes().contains("name")) {
		return itemElem.attribute("name") + "_caption";
	} else if (itemElem.attributes().contains("caption")){
		return "caption_" + itemElem.attribute("caption");
	} else {
		return "caption_default";
	}
}

InputConditionCgnsFile* InputConditionWidgetSet::getCgnsFile(const std::string& name)
{
	auto it = m_cgnsFiles.find(name);
	if (it != m_cgnsFiles.end()) {
		return *it;
	}
	auto cgnsFile = new InputConditionCgnsFile();
	m_cgnsFiles.insert(name, cgnsFile);
	return cgnsFile;
}
