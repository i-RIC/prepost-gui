#ifndef CONNECTIONOUTPUT_H
#define CONNECTIONOUTPUT_H

#include "connectionnode.h"

#include <unordered_map>

class QDomNode;
class QXmlStreamWriter;

class ConnectionOutput : public ConnectionNode
{
public:
	enum class OutputType {
		DumpInterval,
		GlobalOutput,
		GridAttributeComplex,
		BoundaryCondition,
		GridAttribute
	};

	ConnectionOutput();
	ConnectionOutput(const QString& caption, Model* model, OutputType otype, ValueType valueType);
	ConnectionOutput(const QString& caption, Model* model, OutputType otype, const std::string& name, ValueType valueType);
	ConnectionOutput(const QString& caption, Model* model, OutputType otype, const std::string& gridName, const std::string& name, ValueType valueType);
	ConnectionOutput(const QString& caption, Model* model, OutputType otype, const std::string& type, int index, const std::string& name, ValueType valueType);
	ConnectionOutput(const QString& caption, Model* model, OutputType otype, const std::string& gridName, const std::string& type, int index, const std::string& name, ValueType valueType);

	OutputType outputType() const;
	void setOutputType(OutputType type);

	void load(const QDomNode& node, const std::unordered_map<QString, Model*>& models);
	void save(QXmlStreamWriter& writer);

	bool operator==(const ConnectionOutput& output);

	ConnectionOutput* clone() const;

private:
	OutputType m_outputType;
};

#endif // CONNECTIONOUTPUT_H
