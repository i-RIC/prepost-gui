#ifndef MODEL_H
#define MODEL_H

#include <QObject>

#include <string>
#include <unordered_map>

class ConnectionInput;
class ConnectionOutput;
class SolverDefinition;
class SolverDefinitionAbstract;
class iRICMIProject;
class QXmlStreamWriter;

class Model : public QObject
{
	Q_OBJECT

public:
	Model(const std::string& name, const SolverDefinitionAbstract* solverDefinition, iRICMIProject* project);
	~Model();

	const std::string& name() const;
	const SolverDefinitionAbstract* solverDefinition() const;

	std::vector<ConnectionInput*> inputs() const;
	std::vector<ConnectionOutput*> outputs() const;

	QString absolutePath();
	bool createEmptyProjectData();
	void save(QXmlStreamWriter* writer);

	bool buildConnectionNodes(SolverDefinition* solverDefinition);

private:
	bool createEmptyProjectFile();
	bool createEmptyCgnsFile();
        void clearConnectionNodes();

	std::string m_name;
	const SolverDefinitionAbstract* m_solverDefinition;

	std::vector<ConnectionInput*> m_inputs;
	std::vector<ConnectionOutput*> m_outputs;

	iRICMIProject* m_project;

	class ProjectLoader;
};

#endif // MODEL_H
