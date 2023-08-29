#include "connectionoutput.h"
#include "model.h"

#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

ConnectionOutput::ConnectionOutput() :
	ConnectionNode {"", nullptr, ValueType::Integer},
	m_outputType {OutputType::GlobalOutput}
{}

ConnectionOutput::ConnectionOutput(const QString& caption, Model* model, OutputType otype, ValueType valueType) :
	ConnectionNode(caption, model, valueType),
	m_outputType {otype}
{}

ConnectionOutput::ConnectionOutput(const QString& caption, Model* model, OutputType otype, const std::string& name, ValueType valueType) :
	ConnectionNode(caption, model, name, valueType),
	m_outputType {otype}
{}


ConnectionOutput::ConnectionOutput(const QString& caption, Model* model, OutputType otype, const std::string& gridName, const std::string& name, ValueType valueType) :
	ConnectionNode(caption, model, gridName, name, valueType),
	m_outputType {otype}
{}

ConnectionOutput::ConnectionOutput(const QString& caption, Model* model, OutputType otype, const std::string& type, int index, const std::string& name, ValueType valueType) :
	ConnectionNode(caption, model, type, index, name, valueType),
	m_outputType {otype}
{}

ConnectionOutput::ConnectionOutput(const QString& caption, Model* model, OutputType otype, const std::string& gridName, const std::string& type, int index, const std::string& name, ValueType valueType) :
	ConnectionNode(caption, model, gridName, type, index, name, valueType),
	m_outputType {otype}
{}

ConnectionOutput::OutputType ConnectionOutput::outputType() const
{
	return m_outputType;
}

void ConnectionOutput::setOutputType(OutputType type)
{
	m_outputType = type;
}

void ConnectionOutput::load(const QDomNode& node, const std::unordered_map<QString, Model*>& models)
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
		m_outputType = OutputType::DumpInterval;
	} else if (iType == "global") {
		m_outputType = OutputType::GlobalOutput;
	} else if (iType == "boundaryCondition") {
		m_outputType = OutputType::BoundaryCondition;
	} else if (iType == "complexAttribute") {
		m_outputType = OutputType::GridAttributeComplex;
	} else if (iType == "gridAttribute") {
		m_outputType = OutputType::GridAttribute;
	}
	if (m_outputType == OutputType::BoundaryCondition || m_outputType == OutputType::GridAttributeComplex || m_outputType == OutputType::GridAttribute) {
		m_gridName = iRIC::toStr(elem.attribute("gridName"));	}
	if (m_outputType == OutputType::BoundaryCondition || m_outputType == OutputType::GridAttributeComplex) {
		m_type = iRIC::toStr(elem.attribute("typeName"));
	}
	if (m_outputType == OutputType::BoundaryCondition || m_outputType == OutputType::GridAttributeComplex) {
		m_index = iRIC::getIntAttribute(node, "index", 1);
	}
	if (m_outputType != OutputType::DumpInterval) {
		m_name = iRIC::toStr(elem.attribute("name"));
	}
}

void ConnectionOutput::save(QXmlStreamWriter& writer)
{
	writer.writeAttribute("model", m_model->name().c_str());

	QString otype;
	if (m_outputType == OutputType::DumpInterval) {
		otype = "dump_interval";
	} else if (m_outputType == OutputType::GlobalOutput) {
		otype = "global";
	} else if (m_outputType == OutputType::BoundaryCondition) {
		otype = "boundaryCondition";
	} else if (m_outputType == OutputType::GridAttributeComplex) {
		otype = "complexAttribute";
	} else if (m_outputType == OutputType::GridAttribute) {
		otype = "gridAttribute";
	}
	writer.writeAttribute("type", otype);
	if (m_outputType == OutputType::BoundaryCondition || m_outputType == OutputType::GridAttributeComplex || m_outputType == OutputType::GridAttribute) {
		if (m_gridName != "") {
			writer.writeAttribute("gridName", m_gridName.c_str());
		}
	}
	if (m_outputType == OutputType::BoundaryCondition || m_outputType == OutputType::GridAttributeComplex) {
		writer.writeAttribute("typeName", m_type.c_str());
	}
	if (m_outputType == OutputType::BoundaryCondition || m_outputType == OutputType::GridAttributeComplex) {
		iRIC::setIntAttribute(writer, "index", m_index);
	}
	if (m_outputType != OutputType::DumpInterval) {
		writer.writeAttribute("name", m_name.c_str());
	}
}

bool ConnectionOutput::operator==(const ConnectionOutput& output)
{
	if (m_outputType != output.m_outputType) {return false;}

	return ConnectionNode::operator==(output);
}

ConnectionOutput* ConnectionOutput::clone() const
{
	auto ret = new ConnectionOutput();
	*ret = *this;

	return ret;
}
