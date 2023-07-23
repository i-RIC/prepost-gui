#include "../project/iricmiproject.h"
#include "../project/model.h"
#include "existingmodeladddialog.h"
#include "ui_existingmodeladddialog.h"

#include <guicore/solverdef/solverdefinitionabstract.h>
#include <misc/stringtool.h>
#include <misc/versionnumber.h>

#include <unordered_set>

ExistingModelAddDialog::ExistingModelAddDialog(QWidget *parent) :
	QDialog(parent),
	m_models {},
	m_model {},
	ui(new Ui::ExistingModelAddDialog)
{
	ui->setupUi(this);
	ui->tableView->setModel(&m_model);
}

ExistingModelAddDialog::~ExistingModelAddDialog()
{
	delete ui;
}

std::vector<Model*> ExistingModelAddDialog::models() const
{
	return m_models;
}

void ExistingModelAddDialog::setModels(const std::vector<Model*>& models)
{
	m_models = models;

	m_model.setColumnCount(2);
	m_model.setRowCount(models.size());

	m_model.setHeaderData(0, Qt::Horizontal, tr("Folder name"));
	m_model.setHeaderData(1, Qt::Horizontal, tr("Solver"));

	for (int i = 0; i < models.size(); ++i) {
		auto model = models.at(i);

		auto name = new QStandardItem(model->name().c_str());
		name->setCheckable(true);
		name->setCheckState(Qt::Checked);
		m_model.setItem(i, 0, name);

		auto def = model->solverDefinition();
		auto solver = new QStandardItem(QString("%1 %2").arg(def->caption(), def->version().toString()));
		m_model.setItem(i, 1, solver);
	}
}

std::vector<Model*> ExistingModelAddDialog::buildModels(iRICMIProject* project, SolverDefinitionList* solvers)
{
	std::unordered_set<std::string> folders;
	for (auto m : project->models()) {
		folders.insert(m->name());
	}

	std::vector<Model*> models;
	for (auto entry : project->dir().entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
		auto entry_str = iRIC::toStr(entry);
		if (folders.find(entry_str) != folders.end()) {continue;}

		auto solverDef = project->getSolverDefinitionAbstract(entry_str, solvers);
		if (solverDef == nullptr) {continue;}

		auto m = new Model(entry_str, solverDef, project);
		models.push_back(m);
	}

	return models;
}

void ExistingModelAddDialog::accept()
{
	std::vector<Model*> models;
	for (int i = 0; i < m_models.size(); ++i) {
		auto m = m_models.at(i);
		if (m_model.item(i, 0)->checkState() == Qt::Checked) {
			models.push_back(m);
		} else {
			delete m;
		}
	}

	m_models = models;

	QDialog::accept();
}

void ExistingModelAddDialog::reject()
{
	for (auto m : m_models) {
		delete m;
	}
	m_models.clear();

	QDialog::reject();
}
