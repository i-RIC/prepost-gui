#include "connectionnode.h"

ConnectionNode::ConnectionNode(const QString& caption, Model* model, ValueType valueType) :
	ConnectionNode(caption, model, "", valueType)
{}

ConnectionNode::ConnectionNode(const QString& caption, Model* model, const std::string& name, ValueType valueType) :
	ConnectionNode(caption, model, "", name, valueType)
{}

ConnectionNode::ConnectionNode(const QString& caption, Model* model, const std::string& gridName, const std::string& name, ValueType valueType) :
	ConnectionNode(caption, model, gridName, "", 1, name, valueType)
{}

ConnectionNode::ConnectionNode(const QString& caption, Model* model, const std::string& type, int index, const std::string& name, ValueType valueType) :
	ConnectionNode(caption, model, "", type, index, name, valueType)
{}

ConnectionNode::ConnectionNode(const QString& caption, Model* model, const std::string& gridName, const std::string& type, int index, const std::string& name, ValueType valueType) :
	m_caption {caption},
	m_model {model},
	m_gridName {gridName},
	m_type {type},
	m_index {index},
	m_name {name},
	m_valueType {valueType}
{}

QString ConnectionNode::caption() const
{
	return m_caption;
}

void ConnectionNode::setCaption(const QString& caption)
{
	m_caption = caption;
}

Model* ConnectionNode::model() const
{
	return m_model;
}

void ConnectionNode::setModel(Model* model)
{
	m_model = model;
}

std::string ConnectionNode::gridName() const
{
	return m_gridName;
}

void ConnectionNode::setGridName(const std::string& name)
{
	m_gridName = name;
}

std::string ConnectionNode::type() const
{
	return m_type;
}

void ConnectionNode::setType(const std::string& type)
{
	m_type = type;
}

int ConnectionNode::index() const
{
	return m_index;
}

void ConnectionNode::setIndex(int index)
{
	m_index = index;
}

std::string ConnectionNode::name() const
{
	return m_name;
}

void ConnectionNode::setName(const std::string& name)
{
	m_name = name;
}

ConnectionNode::ValueType ConnectionNode::valueType() const
{
	return m_valueType;
}

void ConnectionNode::setValueType(ValueType valueType)
{
	m_valueType = valueType;
}

bool ConnectionNode::operator==(const ConnectionNode& node)
{
	if (m_model != node.m_model) {return false;}
	if (m_gridName != node.m_gridName) {return false;}
	if (m_type != node.m_type) {return false;}
	if (m_index != node.m_index) {return false;}
	if (m_name != node.m_name) {return false;}

	return true;
}
