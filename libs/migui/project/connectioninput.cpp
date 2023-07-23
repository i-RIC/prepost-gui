#include "connectioninput.h"
#include "model.h"

#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

ConnectionInput::ConnectionInput() :
	ConnectionNode {"", nullptr, ValueType::Integer},
	m_inputType {InputType::CalculationCondition}
{}

ConnectionInput::ConnectionInput(const QString& caption, Model* model, InputType itype, ValueType valueType) :
	ConnectionNode(caption, model, valueType),
	m_inputType {itype}
{}

ConnectionInput::ConnectionInput(const QString& caption, Model* model, InputType itype, const std::string& name, ValueType valueType) :
	ConnectionNode(caption, model, name, valueType),
	m_inputType {itype}
{}

ConnectionInput::ConnectionInput(const QString& caption, Model* model, InputType itype, const std::string& gridName, const std::string& name, ValueType valueType) :
	ConnectionNode(caption, model, gridName, name, valueType),
	m_inputType {itype}
{}

ConnectionInput::ConnectionInput(const QString& caption, Model* model, InputType itype, const std::string& type, int index, const std::string& name, ValueType valueType) :
	ConnectionNode(caption, model, type, index, name, valueType),
	m_inputType {itype}
{}

ConnectionInput::ConnectionInput(const QString& caption, Model* model, InputType itype, const std::string& gridName, const std::string& type, int index, const std::string& name, ValueType valueType) :
	ConnectionNode(caption, model, gridName, type, index, name, valueType),
	m_inputType {itype}
{}

ConnectionInput::InputType ConnectionInput::inputType() const
{
	return m_inputType;
}

void ConnectionInput::setInputType(InputType type)
{
	m_inputType = type;
}

void ConnectionInput::load(const QDomNode& node, const std::unordered_map<QString, Model*>& models)
{
	auto elem = node.toElement();

	m_model = nullptr;
	auto modelName = elem.attribute("model");
	auto it = models.find(modelName);
	if (it != models.end()) {
		m_model = it->second;
	}

	auto iType = elem.attribute("type");
	if (iType == "dump_interval") {
		m_inputType = InputType::DumpInterval;
	} else if (iType == "calculationCondition") {
		m_inputType = InputType::CalculationCondition;
	} else if (iType == "boundaryCondition") {
		m_inputType = InputType::BoundaryCondition;
	} else if (iType == "complexAttribute") {
		m_inputType = InputType::GridAttributeComplex;
	} else if (iType == "gridAttribute") {
		m_inputType = InputType::GridAttribute;
	}
	if (m_inputType == InputType::BoundaryCondition || m_inputType == InputType::GridAttributeComplex || m_inputType == InputType::GridAttribute) {
		m_gridName = iRIC::toStr(elem.attribute("gridName"));
	}
	if (m_inputType == InputType::BoundaryCondition || m_inputType == InputType::GridAttributeComplex) {
		m_type = iRIC::toStr(elem.attribute("typeName"));
	}
	if (m_inputType == InputType::BoundaryCondition || m_inputType == InputType::GridAttributeComplex) {
		m_index = iRIC::getIntAttribute(node, "index", 1);
	}
	if (m_inputType != InputType::DumpInterval) {
		m_name = iRIC::toStr(elem.attribute("name"));
	}
}

void ConnectionInput::save(QXmlStreamWriter& writer)
{
	writer.writeAttribute("model", m_model->name().c_str());

	QString itype;
	if (m_inputType == InputType::DumpInterval) {
		itype = "dump_interval";
	} else if (m_inputType == InputType::CalculationCondition) {
		itype = "calculationCondition";
	} else if (m_inputType == InputType::BoundaryCondition) {
		itype = "boundaryCondition";
	} else if (m_inputType == InputType::GridAttributeComplex) {
		itype = "complexAttribute";
	} else if (m_inputType == InputType::GridAttribute) {
		itype = "gridAttribute";
	}
	writer.writeAttribute("type", itype);
	if (m_inputType == InputType::BoundaryCondition || m_inputType == InputType::GridAttributeComplex || m_inputType == InputType::GridAttribute) {
		if (m_gridName != "") {
			writer.writeAttribute("gridName", m_gridName.c_str());
		}
	}
	if (m_inputType == InputType::BoundaryCondition || m_inputType == InputType::GridAttributeComplex) {
		writer.writeAttribute("typeName", m_type.c_str());
	}
	if (m_inputType == InputType::BoundaryCondition || m_inputType == InputType::GridAttributeComplex) {
		iRIC::setIntAttribute(writer, "index", m_index);
	}
	if (m_inputType != InputType::DumpInterval) {
		writer.writeAttribute("name", m_name.c_str());
	}
}

bool ConnectionInput::operator==(const ConnectionInput& input)
{
	if (m_inputType != input.m_inputType) {return false;}

	return ConnectionNode::operator==(input);
}

ConnectionInput* ConnectionInput::clone() const
{
	auto ret = new ConnectionInput();
	*ret = *this;

	return ret;
}
