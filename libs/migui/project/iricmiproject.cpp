#include "connection.h"
#include "connectioninput.h"
#include "connectionoutput.h"
#include "iricmiproject.h"
#include "model.h"
#include "../main/iricmimainwindow.h"

#include <gui/solverdef/solverdefinitionlist.h>
#include <guicore/solverdef/solverdefinition.h>
#include <guicore/solverdef/solverdefinitionabstract.h>
#include <misc/stringtool.h>
#include <misc/versionnumber.h>
#include <misc/xmlsupport.h>

#include <QDomDocument>
#include <QDomElement>
#include <QFile>
#include <QXmlStreamWriter>

iRICMIProject::iRICMIProject(const QString& path, iRICMIMainWindow* window) :
	QObject {window},
	m_isModified {false},
	m_dir {path},
	m_modelsModel {},
	m_mainWindow {window}
{
	window->modelsTable()->setModel(&m_modelsModel);
	window->connectionsTable()->setModel(&m_connectionsModel);

	updateModelsModel();
}

iRICMIProject::~iRICMIProject()
{
	m_mainWindow->modelsTable()->setModel(nullptr);
	m_mainWindow->connectionsTable()->setModel(nullptr);

	for (auto m : m_models) {
		delete m;
	}

	for (auto con : m_connections) {
		delete con;
	}
}

bool iRICMIProject::load(SolverDefinitionList* solvers)
{
	QFile file(fileName());
	bool ok = file.open(QFile::ReadOnly);
	if (! ok) {return false;}

	std::unordered_map<QString, SolverDefinition*> solverDefs;
	std::unordered_map<QString, Model*> modelsMap;

	QDomDocument doc;
	QString errorStr;
	int errorLine, errorColumn;
	ok = doc.setContent(&file, &errorStr, &errorLine, &errorColumn);
	if (! ok) {return false;}

	auto elem = doc.documentElement();
	// models
	auto models = iRIC::getChildNode(elem, "Models");
	if (! models.isNull()) {
		auto childNodes = models.childNodes();
		for (int i = 0; i < childNodes.size(); ++i) {
			auto child = childNodes.at(i).toElement();
			auto folderName = iRIC::toStr(child.attribute("name"));

			auto solverDefAbstract = getSolverDefinitionAbstract(folderName, solvers);
			auto model = new Model(folderName, solverDefAbstract, this);
			auto solverDef = getSolverDefinition(folderName, &solverDefs, solvers);
			model->buildConnectionNodes(solverDef);
			m_models.push_back(model);
			modelsMap.insert({model->name().c_str(), model});
		}
	}
	updateModelsModel();

	// connections
	auto connections = iRIC::getChildNode(elem, "Connections");
	if (! connections.isNull()) {
		auto childNodes = connections.childNodes();
		for (int i = 0; i < childNodes.size(); ++i) {
			auto con = childNodes.at(i).toElement();
			auto from = iRIC::getChildNode(con, "From");
			auto to = iRIC::getChildNode(con, "To");

			auto output = new ConnectionOutput();
			output->load(from, modelsMap);

			auto input = new ConnectionInput();
			input->load(to, modelsMap);

			for (auto o : output->model()->outputs()) {
				if (*output == *o) {
					output->setCaption(o->caption());
				}
			}
			for (auto i : input->model()->inputs()) {
				if (*input == *i) {
					input->setCaption(i->caption());
				}
			}

			auto connection = new Connection(output, input);
			m_connections.push_back(connection);
		}
	}
	updateConnectionsModel();

	for (const auto& pair : solverDefs) {
		delete pair.second;
	}

	return true;
}

bool iRICMIProject::save()
{
	QFile file(fileName());
	bool ok = file.open(QFile::WriteOnly);
	if (! ok) {return false;}

	QXmlStreamWriter writer(&file);
	writer.setAutoFormatting(true);
	writer.writeStartDocument("1.0");
	writer.writeStartElement("iRICMIProject");

	// models
	writer.writeStartElement("Models");
	for (auto m : m_models) {
		writer.writeStartElement("Model");
		m->save(&writer);
		writer.writeEndElement();
	}
	writer.writeEndElement();

	// connections
	writer.writeStartElement("Connections");
	for (auto con : m_connections) {
		writer.writeStartElement("Connection");

		writer.writeStartElement("From");
		con->output()->save(writer);
		writer.writeEndElement();

		writer.writeStartElement("To");
		con->input()->save(writer);
		writer.writeEndElement();

		writer.writeEndElement();
	}

	writer.writeEndElement();

	writer.writeEndElement();
	writer.writeEndDocument();
	file.close();

	return true;
}

bool iRICMIProject::reload(SolverDefinitionList* solvers)
{
	std::unordered_map<QString, SolverDefinition*> solverDefs;

	for (auto m : m_models) {
		auto solverDef = getSolverDefinition(m->name(), &solverDefs, solvers);
		m->buildConnectionNodes(solverDef);
	}

	for (const auto& pair : solverDefs) {
		delete pair.second;
	}

	return true;
}

bool iRICMIProject::isModified() const
{
	return m_isModified;
}

void iRICMIProject::setModified(bool modified)
{
	m_isModified = modified;
}

void iRICMIProject::updateModelsModel()
{
	m_modelsModel.setColumnCount(4);
	m_modelsModel.setRowCount(m_models.size());

	m_modelsModel.setHeaderData(0, Qt::Horizontal, tr("Folder name"));
	m_modelsModel.setHeaderData(1, Qt::Horizontal, tr("Solver"));
	m_modelsModel.setHeaderData(2, Qt::Horizontal, tr("Description"));
	m_modelsModel.setHeaderData(3, Qt::Horizontal, tr("Open"));

	for (int i = 0; i < m_models.size(); ++i) {
		auto model = m_models.at(i);

		auto name = new QStandardItem(model->name().c_str());
		m_modelsModel.setItem(i, 0, name);

		auto def = model->solverDefinition();
		auto solver = new QStandardItem(QString("%1 %2").arg(def->caption(), def->version().toString()));
		m_modelsModel.setItem(i, 1, solver);

		// TODO description
	}
}

void iRICMIProject::updateConnectionsModel()
{
	m_connectionsModel.setColumnCount(4);
	m_connectionsModel.setRowCount(m_connections.size());

	m_connectionsModel.setHeaderData(0, Qt::Horizontal, tr("Output model"));
	m_connectionsModel.setHeaderData(1, Qt::Horizontal, tr("Output value"));
	m_connectionsModel.setHeaderData(2, Qt::Horizontal, tr("Input model"));
	m_connectionsModel.setHeaderData(3, Qt::Horizontal, tr("Input value"));

	for (int i = 0; i < m_connections.size(); ++i) {
		auto con = m_connections.at(i);

		auto outputModel = new QStandardItem(con->output()->model()->name().c_str());
		m_connectionsModel.setItem(i, 0, outputModel);
		auto outputValue = new QStandardItem(con->output()->caption());
		m_connectionsModel.setItem(i, 1, outputValue);
		auto inputModel = new QStandardItem(con->input()->model()->name().c_str());
		m_connectionsModel.setItem(i, 2, inputModel);
		auto inputValue = new QStandardItem(con->input()->caption());
		m_connectionsModel.setItem(i, 3, inputValue);
	}
}

const std::vector<Model*>& iRICMIProject::models() const
{
	return m_models;
}

void iRICMIProject::addModel(Model* model)
{
	m_models.push_back(model);
	m_isModified = true;

	updateModelsModel();
}

void iRICMIProject::removeModel(Model* model)
{
	auto it = std::find(m_models.begin(), m_models.end(), model);
	m_models.erase(it);
	delete model;
	m_isModified = true;

	updateModelsModel();
}

const std::vector<Connection*>& iRICMIProject::connections() const
{
	return m_connections;
}

void iRICMIProject::addConnection(Connection* con)
{
	m_connections.push_back(con);
	m_isModified = true;

	updateConnectionsModel();
}

void iRICMIProject::removeConnection(Connection* con)
{
	auto it = std::find(m_connections.begin(), m_connections.end(), con);
	m_connections.erase(it);
	delete con;
	m_isModified = true;

	updateConnectionsModel();
}


QString iRICMIProject::fileName() const
{
	return m_dir.absoluteFilePath("iricmi_project.xml");
}

const QDir& iRICMIProject::dir() const
{
	return m_dir;
}


SolverDefinitionAbstract* iRICMIProject::getSolverDefinitionAbstract(const std::string& name, SolverDefinitionList* solvers)
{
	auto projectFileName = QString("%1/%2/%3").arg(m_dir.absolutePath()).arg(name.c_str()).arg("project.xml");

	QFile file(projectFileName);
	bool ok = file.open(QFile::ReadOnly);
	if (! ok) {return nullptr;}

	QDomDocument doc;
	QString errorStr;
	int errorLine, errorColumn;
	ok = doc.setContent(&file, &errorStr, &errorLine, &errorColumn);
	if (! ok) {return nullptr;}

	auto elem = doc.documentElement();
	auto solverName = iRIC::toStr(elem.attribute("solverName"));
	VersionNumber solverVersion(elem.attribute("solverVersion"));

	for (auto def : solvers->solverList()) {
		if (def->name() != solverName) {continue;}
		if (! def->version().compatibleWith(solverVersion)) {continue;}

		return def;
	}
	return nullptr;
}

SolverDefinition* iRICMIProject::getSolverDefinition(const std::string& name, std::unordered_map<QString, SolverDefinition*>* solverDefs, SolverDefinitionList* solvers)
{
	auto abst = getSolverDefinitionAbstract(name, solvers);
	if (abst == nullptr) {return nullptr;}

	auto it = solverDefs->find(abst->folderName());
	if (it != solverDefs->end()) {
		return it->second;
	} else {
		auto solverDef = new SolverDefinition(abst->folder().absolutePath(), m_mainWindow->locale());
		solverDefs->insert({abst->folderName(), solverDef});
		return solverDef;
	}

	return nullptr;
}
