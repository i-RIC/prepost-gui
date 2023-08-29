#include "connectioninput.h"
#include "connectionoutput.h"
#include "iricmiproject.h"
#include "model.h"
#include "private/model_projectloader.h"

#include <guicore/project/projectcgnsfile.h>
#include <guicore/solverdef/solverdefinition.h>
#include <guicore/solverdef/solverdefinitionabstract.h>
#include <guicore/solverdef/solverdefinitionboundarycondition.h>
#include <guicore/solverdef/solverdefinitiongridattributedimension.h>
#include <guicore/solverdef/solverdefinitiongridattributeinteger.h>
#include <guicore/solverdef/solverdefinitiongridattributereal.h>
#include <guicore/solverdef/solverdefinitiongridcomplexattribute.h>
#include <guicore/solverdef/solverdefinitiongridoutput.h>
#include <guicore/solverdef/solverdefinitiongridoutputoption.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <guicore/solverdef/solverdefinitiontranslator.h>
#include <misc/stringtool.h>
#include <misc/versionnumber.h>
#include <misc/xmlsupport.h>

#include <h5cgnsfile.h>

#include <QDir>
#include <QDomNode>
#include <QFile>
#include <QXmlStreamWriter>

#include <h5cgnsbase.h>
#include <h5cgnsbc.h>
#include <h5cgnsfile.h>
#include <h5cgnsgridcomplexconditiongroup.h>
#include <h5cgnsgridcomplexconditiontop.h>
#include <h5cgnszone.h>
#include <h5cgnszonebc.h>
#include <iriclib_errorcodes.h>

namespace {

std::string getGroupCaption(iRICLib::H5CgnsConditionGroup* group)
{
	std::string capName = "_caption";
	int length;
	std::vector<char> caption;

	group->readStringLen(capName, &length);
	caption.assign(length + 1, 0);

	group->readString(capName, caption.data());

	return std::string(caption.data());
}

QDomNode getCCNode(const QDomNode& docElem)
{
	auto ccNode = iRIC::getChildNode(docElem, "CalculationCondition");
	if (! ccNode.isNull()) {return ccNode;}

	auto ggcNode = iRIC::getChildNode(docElem, "GridGeneratingCondition");
	if (! ggcNode.isNull()) {return ggcNode;}

	return QDomNode();
}

void buildInputConditionItem(Model* model, const QDomNode& node, std::vector<ConnectionInput*>* inputs, const SolverDefinitionTranslator& translator, const std::vector<QString>& groupCaptions)
{
	auto elem = node.toElement();
	auto defElem = iRIC::getChildNode(node, "Definition").toElement();
	if (defElem.attribute("conditionType") != "functional") {return;}

	auto paramElem = iRIC::getChildNode(defElem, "Parameter").toElement();
	auto paramName = paramElem.attribute("name").toLower();
	auto paramCaption = paramElem.attribute("caption").toLower();

	if (! (paramName == "time" || paramCaption == "time")) {return;}

	QString caption;
	for (const auto& groupCaption : groupCaptions) {
		caption.append(QString("%1 / ").arg(groupCaption));
	}
	caption.append(translator.translate(elem.attribute("caption")));

	auto valueElem = iRIC::getChildNode(defElem, "Value").toElement();
	auto vt = valueElem.attribute("valueType");
	ConnectionInput::ValueType valueType = ConnectionInput::ValueType::Integer;
	if (vt == "real") {valueType = ConnectionInput::ValueType::Real;}

	auto input = new ConnectionInput(caption, model, ConnectionInput::InputType::CalculationCondition, iRIC::toStr(elem.attribute("name")), valueType);
	inputs->push_back(input);
}

void buildInputConditionsRecursive(Model* model, const QDomNode& node, std::vector<ConnectionInput*>* inputs, const SolverDefinitionTranslator& translator, const std::vector<QString>& groupCaptions)
{
	auto childNodes = node.childNodes();
	for (int i = 0; i < childNodes.size(); ++i) {
		auto c = childNodes.at(i);
		if (c.nodeType() == QDomNode::ElementNode && c.nodeName() == "Item") {
			buildInputConditionItem(model, c, inputs, translator, groupCaptions);
		} else {
			if (c.nodeType() == QDomNode::ElementNode && c.nodeName() == "GroupBox") {
				auto groupCaptions2 = groupCaptions;
				groupCaptions2.push_back(translator.translate(c.toElement().attribute("caption")));
				buildInputConditionsRecursive(model, c, inputs, translator, groupCaptions2);
			} else {
				buildInputConditionsRecursive(model, c, inputs, translator, groupCaptions);
			}
		}
	}
}

void buildInputConditions(Model* model, SolverDefinition* def, std::vector<ConnectionInput*>* inputs, const SolverDefinitionTranslator& translator)
{
	auto ccNode = getCCNode(def->document().documentElement());
	if (ccNode.isNull()) {return;}

	std::vector<QString> groupCaptions;
	buildInputConditionsRecursive(model, ccNode, inputs, translator, groupCaptions);
}

void buildInputBounradyConditionItem(Model* model, SolverDefinitionGridType* gt, SolverDefinitionBoundaryCondition* bc, int bcIndex, const std::string& caption, const QDomNode& node, std::vector<ConnectionInput*>* inputs, const SolverDefinitionTranslator& translator, const std::vector<QString>& groupCaptions)
{
	auto elem = node.toElement();
	auto defElem = iRIC::getChildNode(node, "Definition").toElement();
	if (defElem.attribute("conditionType") != "functional") {return;}

	auto paramElem = iRIC::getChildNode(defElem, "Parameter").toElement();
	auto paramName = paramElem.attribute("name").toLower();
	auto paramCaption = paramElem.attribute("caption").toLower();

	if (! (paramName == "time" || paramCaption == "time")) {return;}

	QString valueCaption;
	for (const auto& groupCaption : groupCaptions) {
		valueCaption.append(QString("%1 / ").arg(groupCaption));
	}
	valueCaption.append(translator.translate(elem.attribute("caption")));

	auto valueElem = iRIC::getChildNode(defElem, "Value").toElement();
	auto vt = valueElem.attribute("valueType");
	ConnectionInput::ValueType valueType = ConnectionInput::ValueType::Integer;
	if (vt == "real") {valueType = ConnectionInput::ValueType::Real;}

	auto fullCaption = QString("%1 (%2) / %3").arg(bc->caption(), caption.c_str(), valueCaption);

	auto input = new ConnectionInput(fullCaption, model, ConnectionInput::InputType::BoundaryCondition, gt->name(), bc->name(), bcIndex, iRIC::toStr(elem.attribute("name")), valueType);
	inputs->push_back(input);
}

void buildInputBoundaryConditionsForBcRecursive(Model* model, SolverDefinitionGridType* gt, SolverDefinitionBoundaryCondition* bc, int bcIndex, const std::string& caption, const QDomNode& node, std::vector<ConnectionInput*>* inputs, const SolverDefinitionTranslator& translator, const std::vector<QString>& groupCaptions)
{
	auto childNodes = node.childNodes();
	for (int i = 0; i < childNodes.size(); ++i) {
		auto c = childNodes.at(i);
		if (c.nodeType() == QDomNode::ElementNode && c.nodeName() == "Item") {
			buildInputBounradyConditionItem(model, gt, bc, bcIndex, caption, c, inputs, translator, groupCaptions);
		} else {
			if (c.nodeType() == QDomNode::ElementNode && c.nodeName() == "GroupBox") {
				auto groupCaptions2 = groupCaptions;
				groupCaptions2.push_back(translator.translate(c.toElement().attribute("caption")));
				buildInputBoundaryConditionsForBcRecursive(model, gt, bc, bcIndex, caption, c, inputs, translator, groupCaptions2);
			} else {
				buildInputBoundaryConditionsForBcRecursive(model, gt, bc, bcIndex, caption, c, inputs, translator, groupCaptions);
			}
		}
	}
}

void buildInputBoundaryConditionsForZone(Model* model, SolverDefinitionGridType* gt, iRICLib::H5CgnsZone* zone, std::vector<ConnectionInput*>* inputs, const SolverDefinitionTranslator& translator)
{
	std::vector<QString> groupCaptions;
	auto zoneBc = zone->zoneBc();
	for (auto bc : gt->boundaryConditions()) {
		auto bcCount = zoneBc->bcCount(bc->name());
		for (int i = 0; i < bcCount; ++i) {
			auto cgnsBc = zoneBc->bc(bc->name(), i + 1);
			auto caption = getGroupCaption(cgnsBc);
			buildInputBoundaryConditionsForBcRecursive(model, gt, bc, i + 1, caption, bc->element(), inputs, translator, groupCaptions);
		}
	}
}

void buildInputBoundaryConditionsForGridType(Model* model, SolverDefinitionGridType* gt, const QDomNode& node, iRICLib::H5CgnsFile* cgnsFile, std::vector<ConnectionInput*>* inputs, const SolverDefinitionTranslator& translator)
{
	auto childNodes = node.childNodes();
	for (int i = 0; i < childNodes.size(); ++i) {
		auto c = childNodes.at(i);
		if (c.nodeType() == QDomNode::ElementNode && c.nodeName() == "Region") {
			auto zoneName = iRIC::toStr(c.toElement().attribute("zoneName"));

			auto base = cgnsFile->base(2);
			auto zone = base->zone(zoneName);
			buildInputBoundaryConditionsForZone(model, gt, zone, inputs, translator);
		}
	}
}

void buildInputBoundaryConditionsRecursive(Model* model, SolverDefinition* def, const QDomNode& node, iRICLib::H5CgnsFile* cgnsFile, std::vector<ConnectionInput*>* inputs, const SolverDefinitionTranslator& translator)
{
	auto childNodes = node.childNodes();
	for (int i = 0; i < childNodes.size(); ++i) {
		auto c = childNodes.at(i);
		if (c.nodeType() == QDomNode::ElementNode && c.nodeName() == "GridType") {
			bool built = false;
			auto name = iRIC::toStr(c.toElement().attribute("name"));
			for (auto gType : def->gridTypes()) {
				if (gType->name() == name) {
					buildInputBoundaryConditionsForGridType(model, gType, c, cgnsFile, inputs, translator);
					built = true;
					break;
				}
			}
			if (! built) {
				auto gridTypes = def->gridTypes();
				if (gridTypes.size() > 0) {
					buildInputBoundaryConditionsForGridType(model, gridTypes.at(0), c, cgnsFile, inputs, translator);
				}
			}
		} else {
			buildInputBoundaryConditionsRecursive(model, def, c, cgnsFile, inputs, translator);
		}
	}
}

void buildInputBoundaryConditions(Model* model, SolverDefinition* def, const QDomNode& preNode, iRICLib::H5CgnsFile* cgnsFile, std::vector<ConnectionInput*>* inputs, const SolverDefinitionTranslator& translator)
{
	buildInputBoundaryConditionsRecursive(model, def, preNode, cgnsFile, inputs, translator);
}

void buildInputComplexConditionItem(Model* model, SolverDefinitionGridComplexAttribute* att, int complexIndex, const std::string& caption, const QDomNode& node, std::vector<ConnectionInput*>* inputs, const SolverDefinitionTranslator& translator, const std::vector<QString>& groupCaptions)
{
	auto elem = node.toElement();
	auto defElem = iRIC::getChildNode(node, "Definition").toElement();
	if (defElem.attribute("conditionType") != "functional") {return;}

	auto paramElem = iRIC::getChildNode(defElem, "Parameter").toElement();
	auto paramName = paramElem.attribute("name").toLower();
	auto paramCaption = paramElem.attribute("caption").toLower();

	if (! (paramName == "time" || paramCaption == "time")) {return;}

	QString valueCaption;
	for (const auto& groupCaption : groupCaptions) {
		valueCaption.append(QString("%1 / ").arg(groupCaption));
	}
	valueCaption.append(translator.translate(elem.attribute("caption")));

	auto valueElem = iRIC::getChildNode(defElem, "Value").toElement();
	auto vt = valueElem.attribute("valueType");
	ConnectionInput::ValueType valueType = ConnectionInput::ValueType::Integer;
	if (vt == "real") {valueType = ConnectionInput::ValueType::Real;}

	auto fullCaption = QString("%1 (%2) / %3").arg(att->caption(), caption.c_str(), valueCaption);

	auto input = new ConnectionInput(fullCaption, model, ConnectionInput::InputType::BoundaryCondition, att->name(), complexIndex, iRIC::toStr(elem.attribute("name")), valueType);
	inputs->push_back(input);
}

void buildInputComplexConditionsRecursive(Model* model, SolverDefinitionGridComplexAttribute* att, int complexIndex, const std::string& caption, const QDomNode& node, std::vector<ConnectionInput*>* inputs, const SolverDefinitionTranslator& translator, const std::vector<QString>& groupCaptions)
{
	auto childNodes = node.childNodes();
	for (int i = 0; i < childNodes.size(); ++i) {
		auto c = childNodes.at(i);
		if (c.nodeType() == QDomNode::ElementNode && c.nodeName() == "Item") {
			buildInputComplexConditionItem(model, att, complexIndex, caption, c, inputs, translator, groupCaptions);
		} else {
			if (c.nodeType() == QDomNode::ElementNode && c.nodeName() == "GroupBox") {
				auto groupCaptions2 = groupCaptions;
				groupCaptions2.push_back(translator.translate(c.toElement().attribute("caption")));
				buildInputComplexConditionsRecursive(model, att, complexIndex, caption, c, inputs, translator, groupCaptions2);
			} else {
				buildInputComplexConditionsRecursive(model, att, complexIndex, caption, c, inputs, translator, groupCaptions);
			}
		}
	}
}

void buildInputComplexConditionsForGridType(Model* model, SolverDefinitionGridType* gt, iRICLib::H5CgnsBase* cgnsBase, std::vector<ConnectionInput*>* inputs, const SolverDefinitionTranslator& translator)
{
	std::vector<QString> groupCaptions;
	auto gccTop = cgnsBase->gccTop();
	for (auto complexAtt : gt->gridComplexAttributes()) {
		auto compGroup = gccTop->group(complexAtt->name());
		int compCount = compGroup->itemCount();
		for (int i = 0; i < compCount; ++i) {
			auto group = compGroup->item(i + 1);
			auto caption = getGroupCaption(group);

			buildInputComplexConditionsRecursive(model, complexAtt, i + 1, caption, complexAtt->element(), inputs, translator, groupCaptions);
		}
	}
}

void buildInputComplexConditionsRecursive(Model* model, SolverDefinition* def, const QDomNode& node, iRICLib::H5CgnsBase* cgnsBase, std::vector<ConnectionInput*>* inputs, const SolverDefinitionTranslator& translator)
{
	auto childNodes = node.childNodes();
	for (int i = 0; i < childNodes.size(); ++i) {
		auto c = childNodes.at(i);
		if (c.nodeType() == QDomNode::ElementNode && c.nodeName() == "GridType") {
			bool built = false;
			auto name = iRIC::toStr(c.toElement().attribute("name"));
			for (auto gType : def->gridTypes()) {
				if (gType->name() == name) {
					buildInputComplexConditionsForGridType(model, gType, cgnsBase, inputs, translator);
					built = true;
					break;
				}
			}
			if (! built) {
				auto gridTypes = def->gridTypes();
				if (gridTypes.size() > 0) {
					buildInputComplexConditionsForGridType(model, gridTypes.at(0), cgnsBase, inputs, translator);
				}
			}
		} else {
			buildInputComplexConditionsRecursive(model, def, c, cgnsBase, inputs, translator);
		}
	}
}

void buildInputComplexConditions(Model* model, SolverDefinition* def, const QDomNode& preNode, iRICLib::H5CgnsFile* cgnsFile, std::vector<ConnectionInput*>* inputs, const SolverDefinitionTranslator& translator)
{
	buildInputComplexConditionsRecursive(model, def, preNode, cgnsFile->ccBase(), inputs, translator);
}

void buildInputGridAttributeItem(Model* model, SolverDefinitionGridType* gt, SolverDefinitionGridAttribute* att, std::vector<ConnectionInput*>* inputs, bool multi)
{
	bool hasTimeDimension = false;

	for (auto dim : att->dimensions()) {
		if (dim->name() == "Time") {
			hasTimeDimension = true;
			break;
		}
	}
	if (! hasTimeDimension) {return;}

	ConnectionInput::ValueType valueType = ConnectionInput::ValueType::Integer;
	if (dynamic_cast<SolverDefinitionGridAttributeReal*> (att) != nullptr) {
		valueType = ConnectionInput::ValueType::Real;
	}

	auto gtCaption = gt->caption();

	QString caption;
	ConnectionInput* input = nullptr;
	if (multi) {
		caption = QString("%1 / %2").arg(gtCaption, att->caption());
		input = new ConnectionInput(caption, model, ConnectionInput::InputType::GridAttribute, gt->name(), att->name(), valueType);
	} else {
		input = new ConnectionInput(att->caption(), model, ConnectionInput::InputType::GridAttribute, gt->name(), att->name(), valueType);
	}
	inputs->push_back(input);
}

void buildInputGridAttributesForGridType(Model* model, SolverDefinitionGridType* gt, std::vector<ConnectionInput*>* inputs, bool multi)
{
	for (auto att : gt->gridAttributes()) {
		buildInputGridAttributeItem(model, gt, att, inputs, multi);
	}
}

void buildInputGridAttributes(Model* model, SolverDefinition* def, const QDomNode& preNode, std::vector<ConnectionInput*>* inputs)
{
	auto gridTypes = def->gridTypes();
	bool multi = gridTypes.size() > 1;
	for (auto gridType : gridTypes) {
		buildInputGridAttributesForGridType(model, gridType, inputs, multi);
	}
}

QDomNode getGONode(const QDomNode& docElem)
{
	return iRIC::getChildNode(docElem, "GlobalOutput");
}

void buildGlobalOutputItem(Model* model, const QDomNode& node, std::vector<ConnectionOutput*>* outputs, const SolverDefinitionTranslator& translator)
{
	auto elem = node.toElement();

	auto caption = translator.translate(elem.attribute("caption"));
	auto defElem = iRIC::getChildNode(node, "Definition").toElement();
	auto vt = defElem.attribute("valueType");
	ConnectionOutput::ValueType valueType = ConnectionOutput::ValueType::Integer;
	if (vt == "real") {valueType = ConnectionOutput::ValueType::Real;}

	auto output = new ConnectionOutput(caption, model, ConnectionOutput::OutputType::GlobalOutput, iRIC::toStr(elem.attribute("name")), valueType);
	outputs->push_back(output);
}

void buildGlobalOutputRecursive(Model* model, const QDomNode& node, std::vector<ConnectionOutput*>* outputs, const SolverDefinitionTranslator& translator)
{
	auto childNodes = node.childNodes();
	for (int i = 0; i < childNodes.size(); ++i) {
		auto c = childNodes.at(i);
		if (c.nodeType() == QDomNode::ElementNode && c.nodeName() == "Output") {
			buildGlobalOutputItem(model, c, outputs, translator);
		} else {
			buildGlobalOutputRecursive(model, c, outputs, translator);
		}
	}
}

void buildGlobalOutputs(Model* model, SolverDefinition* def, std::vector<ConnectionOutput*>* outputs, const SolverDefinitionTranslator translator)
{
	auto goNode = getGONode(def->document().documentElement());
	if (goNode.isNull()) {return;}

	buildGlobalOutputRecursive(model, goNode, outputs, translator);
}

void buildOutputBounradyConditionItem(Model* model, SolverDefinitionGridType* gt, SolverDefinitionBoundaryCondition* bc, int bcIndex, const std::string& caption, const QDomNode& node, std::vector<ConnectionOutput*>* outputs, const SolverDefinitionTranslator& translator)
{
	auto elem = node.toElement();

	auto valueCaption = translator.translate(elem.attribute("caption"));
	auto defElem = iRIC::getChildNode(node, "Definition").toElement();
	auto vt = defElem.attribute("valueType");
	ConnectionOutput::ValueType valueType = ConnectionOutput::ValueType::Integer;
	if (vt == "real") {valueType = ConnectionOutput::ValueType::Real;}

	auto fullCaption = QString("%1 (%2) / %3").arg(bc->caption(), caption.c_str(), valueCaption);

	auto output = new ConnectionOutput(fullCaption, model, ConnectionOutput::OutputType::BoundaryCondition, gt->name(), bc->name(), bcIndex, iRIC::toStr(elem.attribute("name")), valueType);
	outputs->push_back(output);
}

void buildOutputBoundaryConditionsForBcRecursive(Model* model, SolverDefinitionGridType* gt, SolverDefinitionBoundaryCondition* bc, int bcIndex, const std::string& caption, const QDomNode& node, std::vector<ConnectionOutput*>* outputs, const SolverDefinitionTranslator& translator)
{
	auto childNodes = node.childNodes();
	for (int i = 0; i < childNodes.size(); ++i) {
		auto c = childNodes.at(i);
		if (c.nodeType() == QDomNode::ElementNode && c.nodeName() == "Output") {
			buildOutputBounradyConditionItem(model, gt, bc, bcIndex, caption, c, outputs, translator);
		} else {
			buildOutputBoundaryConditionsForBcRecursive(model, gt, bc, bcIndex, caption, c, outputs, translator);
		}
	}
}

void buildOutputBoundaryConditionsForZone(Model* model, SolverDefinitionGridType* gt, iRICLib::H5CgnsZone* zone, std::vector<ConnectionOutput*>* outputs, const SolverDefinitionTranslator& translator)
{
	auto zoneBc = zone->zoneBc();
	for (auto bc : gt->boundaryConditions()) {
		auto bcCount = zoneBc->bcCount(bc->name());
		for (int i = 0; i < bcCount; ++i) {
			auto cgnsBc = zoneBc->bc(bc->name(), i + 1);
			auto caption = getGroupCaption(cgnsBc);
			buildOutputBoundaryConditionsForBcRecursive(model, gt, bc, i + 1, caption, bc->element(), outputs, translator);
		}
	}
}

void buildOutputBoundaryConditionsForGridType(Model* model, SolverDefinitionGridType* gt, const QDomNode& node, iRICLib::H5CgnsFile* cgnsFile, int* nextGridId, std::vector<ConnectionOutput*>* outputs, const SolverDefinitionTranslator& translator)
{
	auto childNodes = node.childNodes();
	for (int i = 0; i < childNodes.size(); ++i) {
		auto c = childNodes.at(i);
		if (c.nodeType() == QDomNode::ElementNode && c.nodeName() == "Region") {
			auto zoneName = iRIC::toStr(c.toElement().attribute("zoneName"));

			auto base = cgnsFile->base(2);
			auto zone = base->zone(zoneName);
			buildOutputBoundaryConditionsForZone(model, gt, zone, outputs, translator);

			++ *nextGridId;
		}
	}
}

void buildOutputBoundaryConditionsRecursive(Model* model, SolverDefinition* def, const QDomNode& node, iRICLib::H5CgnsFile* cgnsFile, int* nextGridId, std::vector<ConnectionOutput*>* outputs, const SolverDefinitionTranslator& translator)
{
	auto childNodes = node.childNodes();
	for (int i = 0; i < childNodes.size(); ++i) {
		auto c = childNodes.at(i);
		if (c.nodeType() == QDomNode::ElementNode && c.nodeName() == "GridType") {
			bool built = false;
			auto name = iRIC::toStr(c.toElement().attribute("name"));
			for (auto gType : def->gridTypes()) {
				if (gType->name() == name) {
					buildOutputBoundaryConditionsForGridType(model, gType, c, cgnsFile, nextGridId, outputs, translator);
					built = true;
					break;
				}
			}
			if (! built) {
				auto gridTypes = def->gridTypes();
				if (gridTypes.size() > 0) {
					buildOutputBoundaryConditionsForGridType(model, gridTypes.at(0), c, cgnsFile, nextGridId, outputs, translator);
				}
			}
		} else {
			buildOutputBoundaryConditionsRecursive(model, def, c, cgnsFile, nextGridId, outputs, translator);
		}
	}
}


void buildOutputBoundaryConditions(Model* model, SolverDefinition* def, const QDomNode& preNode, iRICLib::H5CgnsFile* cgnsFile, std::vector<ConnectionOutput*>* outputs, const SolverDefinitionTranslator& translator)
{
	int nextGridId = 1;
	buildOutputBoundaryConditionsRecursive(model, def, preNode, cgnsFile, &nextGridId, outputs, translator);
}

void buildOutputComplexConditionItem(Model* model, SolverDefinitionGridComplexAttribute* att, int complexIndex, const std::string& caption, const QDomNode& node, std::vector<ConnectionOutput*>* outputs, const SolverDefinitionTranslator& translator)
{
	auto elem = node.toElement();

	auto valueCaption = translator.translate(elem.attribute("caption"));
	auto defElem = iRIC::getChildNode(node, "Definition").toElement();
	auto vt = defElem.attribute("valueType");
	ConnectionOutput::ValueType valueType = ConnectionOutput::ValueType::Integer;
	if (vt == "real") {valueType = ConnectionOutput::ValueType::Real;}

	auto fullCaption = QString("%1 (%2) / %3").arg(att->caption(), caption.c_str(), valueCaption);

	auto output = new ConnectionOutput(fullCaption, model, ConnectionOutput::OutputType::BoundaryCondition, att->name(), complexIndex, iRIC::toStr(elem.attribute("name")), valueType);
	outputs->push_back(output);
}

void buildOutputComplexConditionsRecursive(Model* model, SolverDefinitionGridComplexAttribute* att, int complexIndex, const std::string& caption, const QDomNode& node, std::vector<ConnectionOutput*>* outputs, const SolverDefinitionTranslator& translator)
{
	auto childNodes = node.childNodes();
	for (int i = 0; i < childNodes.size(); ++i) {
		auto c = childNodes.at(i);
		if (c.nodeType() == QDomNode::ElementNode && c.nodeName() == "Output") {
			buildOutputComplexConditionItem(model, att, complexIndex, caption, c, outputs, translator);
		} else {
			buildOutputComplexConditionsRecursive(model, att, complexIndex, caption, c, outputs, translator);
		}
	}
}

void buildOutputComplexConditionsForGridType(Model* model, SolverDefinitionGridType* gt, iRICLib::H5CgnsBase* cgnsBase, std::vector<ConnectionOutput*>* outputs, const SolverDefinitionTranslator& translator)
{
	auto gccTop = cgnsBase->gccTop();
	for (auto complexAtt : gt->gridComplexAttributes()) {
		auto compGroup = gccTop->group(complexAtt->name());
		int compCount = compGroup->itemCount();
		for (int i = 0; i < compCount; ++i) {
			auto group = compGroup->item(i + 1);
			auto caption = getGroupCaption(group);

			buildOutputComplexConditionsRecursive(model, complexAtt, i + 1, caption, complexAtt->element(), outputs, translator);
		}
	}
}

void buildOutputComplexConditionsRecursive(Model* model, SolverDefinition* def, const QDomNode& node, iRICLib::H5CgnsBase* cgnsBase, std::vector<ConnectionOutput*>* outputs, const SolverDefinitionTranslator& translator)
{
	auto childNodes = node.childNodes();
	for (int i = 0; i < childNodes.size(); ++i) {
		auto c = childNodes.at(i);
		if (c.nodeType() == QDomNode::ElementNode && c.nodeName() == "GridType") {
			bool built = false;
			auto name = iRIC::toStr(c.toElement().attribute("name"));
			for (auto gType : def->gridTypes()) {
				if (gType->name() == name) {
					buildOutputComplexConditionsForGridType(model, gType, cgnsBase, outputs, translator);
					built = true;
					break;
				}
			}
			if (! built) {
				auto gridTypes = def->gridTypes();
				if (gridTypes.size() > 0) {
					buildOutputComplexConditionsForGridType(model, gridTypes.at(0), cgnsBase, outputs, translator);
				}
			}
		} else {
			buildOutputComplexConditionsRecursive(model, def, c, cgnsBase, outputs, translator);
		}
	}
}

void buildOutputComplexConditions(Model* model, SolverDefinition* def, const QDomNode& preNode, iRICLib::H5CgnsFile* cgnsFile, std::vector<ConnectionOutput*>* outputs, const SolverDefinitionTranslator& translator)
{
	buildOutputComplexConditionsRecursive(model, def, preNode, cgnsFile->ccBase(), outputs, translator);
}

void buildOutputGridOutputsForGridType(Model* model, SolverDefinitionGridType* gt, std::vector<ConnectionOutput*>* outputs, const SolverDefinitionTranslator& translator, bool multi)
{
	auto gtCaption = gt->caption();

	for (auto o : gt->outputs()) {
		if (o->position() == SolverDefinitionGridOutput::Position::Particle) {continue;}
		if (o->position() == SolverDefinitionGridOutput::Position::PolyData) {continue;}

		ConnectionOutput::ValueType vt = ConnectionOutput::ValueType::Real;
		auto o2 = dynamic_cast<SolverDefinitionGridOutputOption*> (o);
		if (o2 != nullptr) {
			vt = ConnectionOutput::ValueType::Integer;
		}
		QString caption;
		ConnectionOutput* output = nullptr;
		if (multi) {
			caption = QString("%1 / %2").arg(gtCaption, o->caption());
			output = new ConnectionOutput(caption, model, ConnectionOutput::OutputType::GridAttribute, gt->name(), o->name(), vt);
		} else {
			output = new ConnectionOutput(o->caption(), model, ConnectionOutput::OutputType::GridAttribute, o->name(), vt);
		}
		outputs->push_back(output);
	}
}

void buildOutputGridOutputsRecursive(Model* model, SolverDefinition* def, const QDomNode& node, std::vector<ConnectionOutput*>* outputs, const SolverDefinitionTranslator& translator)
{
	auto gridTypes = def->gridTypes();
	bool multi = gridTypes.size() > 1;
	for (auto gridType : gridTypes) {
		buildOutputGridOutputsForGridType(model, gridType, outputs, translator, multi);
	}
}

void buildOutputGridOutputs(Model* model, SolverDefinition* def, const QDomNode& preNode, std::vector<ConnectionOutput*>* outputs, const SolverDefinitionTranslator& translator)
{
	buildOutputGridOutputsRecursive(model, def, preNode, outputs, translator);
}

} // namespace

Model::Model(const std::string& name, const SolverDefinitionAbstract* solverDefinition, iRICMIProject* project) :
	m_name {name},
	m_solverDefinition {solverDefinition},
	m_project {project}
{}

Model::~Model()
{
	clearConnectionNodes();
}

const std::string& Model::name() const
{
	return m_name;
}

const SolverDefinitionAbstract* Model::solverDefinition() const
{
	return m_solverDefinition;
}

std::vector<ConnectionInput*> Model::inputs() const
{
	return m_inputs;
}

std::vector<ConnectionOutput*> Model::outputs() const
{
	return m_outputs;
}

QString Model::absolutePath()
{
	return m_project->dir().absoluteFilePath(name().c_str());
}

bool Model::createEmptyProjectData()
{
	if (! m_project->dir().mkdir(name().c_str())) {return false;}
	if (! createEmptyProjectFile()) {return false;}
	if (! createEmptyCgnsFile()) {return false;}

	return true;
}

void Model::save(QXmlStreamWriter* writer)
{
	writer->writeAttribute("name", m_name.c_str());
	writer->writeAttribute("nodes", "1");
}

bool Model::buildConnectionNodes(SolverDefinition* solverDefinition)
{
	clearConnectionNodes();

	ProjectLoader projectLoader(this);
	bool ok = projectLoader.load();
	if (! ok) {return false;}

	QDir dir(absolutePath());
	auto cgnsFName = dir.absoluteFilePath("Case1.cgn");

	iRICLib::H5CgnsFile cgnsFile(iRIC::toStr(cgnsFName), iRICLib::H5CgnsFile::Mode::OpenReadOnly);

	auto translator = solverDefinition->buildTranslator();

	buildInputConditions(this, solverDefinition, &m_inputs, translator);
	buildInputBoundaryConditions(this, solverDefinition, projectLoader.projectPreProcessorNode(), &cgnsFile, &m_inputs, translator);
	buildInputComplexConditions(this, solverDefinition, projectLoader.projectPreProcessorNode(), &cgnsFile, &m_inputs, translator);
	buildInputGridAttributes(this, solverDefinition, projectLoader.projectPreProcessorNode(), &m_inputs);
	m_inputs.push_back(new ConnectionInput(Model::tr("Output time interval"), this, ConnectionInput::InputType::DumpInterval, ConnectionInput::ValueType::Real));

	buildGlobalOutputs(this, solverDefinition, &m_outputs, translator);
	buildOutputBoundaryConditions(this, solverDefinition, projectLoader.projectPreProcessorNode(), &cgnsFile, &m_outputs, translator);
	buildOutputComplexConditions(this, solverDefinition, projectLoader.projectPreProcessorNode(), &cgnsFile, &m_outputs, translator);
	buildOutputGridOutputs(this, solverDefinition, projectLoader.projectPreProcessorNode(), &m_outputs, translator);
	m_outputs.push_back(new ConnectionOutput(Model::tr("Output time interval"), this, ConnectionOutput::OutputType::DumpInterval, ConnectionOutput::ValueType::Real));

	return true;
}

bool Model::createEmptyProjectFile()
{
	QFile file(QDir(absolutePath()).absoluteFilePath("project.xml"));
	bool ok = file.open(QFile::WriteOnly);
	if (! ok) {return false;}

	QXmlStreamWriter writer(&file);

	writer.setAutoFormatting(true);
	writer.writeStartDocument("1.0");

	writer.writeStartElement("iRICProject");
	writer.writeAttribute("version", "4.0.0.6868"); // TODO: fix this to set value dynamically
	writer.writeAttribute("solverName", m_solverDefinition->name().c_str());
	writer.writeAttribute("solverVersion", m_solverDefinition->version().toString());
	writer.writeEndElement();

	writer.writeEndDocument();

	file.close();
	return true;
}

bool Model::createEmptyCgnsFile()
{
	auto fname = QDir(absolutePath()).absoluteFilePath("Case1.cgn");
	try {
		iRICLib::H5CgnsFile cgnsFile(iRIC::toStr(fname), iRICLib::H5CgnsFile::Mode::Create);
		int ier = ProjectCgnsFile::writeSolverInfo(&cgnsFile, *m_solverDefinition);
		return (ier == IRIC_NO_ERROR);
		return true;
	} catch (...) {
		return false;
	}
}

void Model::clearConnectionNodes()
{
	for (auto con : m_inputs) {delete con;}
	m_inputs.clear();

	for (auto con : m_outputs) {delete con;}
	m_outputs.clear();
}
