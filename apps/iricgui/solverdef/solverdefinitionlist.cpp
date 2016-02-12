#include "solverdefinitionlist.h"
#include "solverdefinitionlistdialog.h"

#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/solverdef/solverdefinition.h>
#include <guicore/solverdef/solverdefinitionabstract.h>
#include <misc/errormessage.h>

#include <QAction>
#include <QDir>
#include <QFileSystemWatcher>
#include <QMessageBox>

SolverDefinitionList::SolverDefinitionList(const QString& installDir, const QLocale& locale, QObject* parent) :
	QObject(parent)
{
	m_locale = locale;
	QString solversFolder("solvers");

	QDir inst(installDir);
	inst.cdUp();
	inst.cdUp();
	m_targetDirectory = inst.absoluteFilePath(solversFolder);
	if (! QDir(m_targetDirectory).exists()) {
		inst.mkdir(solversFolder);
	}
	// initially, dialog is not prepared.
	m_dialog = nullptr;

	// setup filesystem watcher
	QFileSystemWatcher* watcher = new QFileSystemWatcher(this);
	watcher->addPath(m_targetDirectory);
	connect(watcher, SIGNAL(directoryChanged(QString)), this, SLOT(updateSolverList()));
	// update solver list.
	updateSolverList();
}

SolverDefinitionList::~SolverDefinitionList()
{
	clean();
}

void SolverDefinitionList::updateSolverList()
{
	clean();
	QWidget* parentWidget = dynamic_cast<QWidget*>(parent());

	QDir solversdir(m_targetDirectory);
	QStringList subdirs = solversdir.entryList(QStringList(), QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);

	for (const QString& subdir : subdirs) {
		QString defFileName = QDir(solversdir.absoluteFilePath(subdir)).absoluteFilePath(SolverDefinition::FILENAME);
		if (QFile::exists(defFileName)) {
			// definition.xml exists.
			try {
				SolverDefinitionAbstract* abst = new SolverDefinitionAbstract(solversdir.absoluteFilePath(subdir), m_locale, this);
				m_solverList.push_back(abst);
			} catch (ErrorMessage& e) {
				QMessageBox::warning(parentWidget, tr("Warning"), tr("Error occured while loading solver definition file in folder \"%1\". This solver is ignored.\n%2").arg(subdir).arg(e));
			}
		}
	}
	// inform the change of solverList
	emit updated();
	emit updated(this);
}

void SolverDefinitionList::clean()
{
	for (auto def : m_solverList) {
		delete def;
	}
	m_solverList.clear();
}

QString SolverDefinitionList::absoluteSolverPath(const QString& solverFolder)
{
	return QDir(m_targetDirectory).absoluteFilePath(solverFolder);
}

const std::vector<SolverDefinitionAbstract*>& SolverDefinitionList::solverList() const
{
	return m_solverList;
}

std::vector<QAction*> SolverDefinitionList::actionList()
{
	std::vector<QAction*> ret;
	for (SolverDefinitionAbstract* def : m_solverList) {
		QAction* act = new QAction(def->caption(), this);
		ret.push_back(act);
	}
	return ret;
}

SolverDefinitionListDialog* SolverDefinitionList::dialog(QWidget* parent)
{
	if (m_dialog == nullptr) {
		m_dialog = new SolverDefinitionListDialog(this, parent);
	}
	m_dialog->setup();
	return m_dialog;
}

QString SolverDefinitionList::supportingSolverFolder(ProjectData* p)
{
	for (SolverDefinitionAbstract* solver : m_solverList) {
		if (solver->name() == p->mainfile()->solverName() &&
				solver->version().compatibleWith(p->mainfile()->solverVersion())) {
			return solver->folderName();
		}
	}
	return QString::null;
}
