#ifndef CONNECTIONINPUT_H
#define CONNECTIONINPUT_H

#include "connectionnode.h"

#include <unordered_map>

class QDomNode;
class QXmlStreamWriter;

class ConnectionInput : public ConnectionNode
{
public:
	enum class InputType {
		DumpInterval,
		CalculationCondition,
		GridAttributeComplex,
		BoundaryCondition,
		GridAttribute
	};

	ConnectionInput();
	ConnectionInput(const QString& caption, Model* model, InputType itype, ValueType valueType);
	ConnectionInput(const QString& caption, Model* model, InputType itype, const std::string& name, ValueType valueType);
	ConnectionInput(const QString& caption, Model* model, InputType itype, const std::string& gridName, const std::string& name, ValueType valueType);
	ConnectionInput(const QString& caption, Model* model, InputType itype, const std::string& type, int index, const std::string& name, ValueType valueType);
	ConnectionInput(const QString& caption, Model* model, InputType itype, const std::string& gridName, const std::string& type, int index, const std::string& name, ValueType valueType);

	InputType inputType() const;
	void setInputType(InputType type);

	void load(const QDomNode& node, const std::unordered_map<QString, Model*>& models);
	void save(QXmlStreamWriter& writer);

	bool operator==(const ConnectionInput& input);

	ConnectionInput* clone() const;

private:
	InputType m_inputType;
};

#endif // CONNECTIONINPUT_H
