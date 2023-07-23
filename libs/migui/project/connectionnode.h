#ifndef CONNECTIONNODE_H
#define CONNECTIONNODE_H

#include <QString>

class Model;

class ConnectionNode
{
public:
	enum class ValueType {
		Integer,
		Real
	};

	ConnectionNode(const QString& caption, Model* model, ValueType valueType);
	ConnectionNode(const QString& caption, Model* model, const std::string& name, ValueType valueType);
	ConnectionNode(const QString& caption, Model* model, const std::string& gridName, const std::string& name, ValueType valueType);
	ConnectionNode(const QString& caption, Model* model, const std::string& type, int index, const std::string& name, ValueType valueType);
	ConnectionNode(const QString& caption, Model* model, const std::string& gridName, const std::string& type, int index, const std::string& name, ValueType valueType);

	QString caption() const;
	void setCaption(const QString& caption);

	Model* model() const;
	void setModel(Model* model);

	std::string gridName() const;
	void setGridName(const std::string& name);

	std::string type() const;
	void setType(const std::string& type);

	int index() const;
	void setIndex(int index);

	std::string name() const;
	void setName(const std::string& name);

	ValueType valueType() const;
	void setValueType(ValueType valueType);

	bool operator==(const ConnectionNode& node);

protected:
	QString m_caption;

	Model* m_model;
	std::string m_gridName;
	std::string m_type;
	int m_index;
	std::string m_name;
	ValueType m_valueType;
};

#endif // CONNECTIONNODE_H
