#ifndef IRICMIPROJECT_H
#define IRICMIPROJECT_H

#include <QDir>
#include <QObject>
#include <QStandardItemModel>
#include <QString>

#include <unordered_map>

class iRICMIMainWindow;
class Connection;
class Model;
class SolverDefinition;
class SolverDefinitionAbstract;
class SolverDefinitionList;

class iRICMIProject : public QObject
{
	Q_OBJECT

public:
	iRICMIProject(const QString& path, iRICMIMainWindow* window);
	~iRICMIProject();

	bool load(SolverDefinitionList* solvers, QWidget* w);
	bool save();
	bool reload(SolverDefinitionList* solvers);

	bool isModified() const;
	void setModified(bool modified);

	void updateModelsModel();
	void updateConnectionsModel();

	const std::vector<Model*>& models() const;
	void addModel(Model* model);
	void removeModel(Model* model);

	const std::vector<Connection*>& connections() const;
	void addConnection(Connection* con);
	void removeConnection(Connection* con);

	SolverDefinitionAbstract* getSolverDefinitionAbstract(const std::string& name, SolverDefinitionList* solvers);

	QString fileName() const;
	const QDir& dir() const;

private:
	SolverDefinition* getSolverDefinition(const std::string& name, std::unordered_map<QString, SolverDefinition*>* solverDefs, SolverDefinitionList* solvers);

	std::vector<Model*> m_models;
	std::vector<Connection*> m_connections;

	bool m_isModified;
	QDir m_dir;
	QStandardItemModel m_modelsModel;
	QStandardItemModel m_connectionsModel;

	iRICMIMainWindow* m_mainWindow;
};

#endif // IRICMIPROJECT_H
