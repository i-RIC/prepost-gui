#include "solverdefinitionlist.h"
#include "solverdefinitionlistdialog.h"

#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/solverdef/solverdefinition.h>
#include <guicore/solverdef/solverdefinitionabstract.h>
#include <misc/errormessage.h>
#include <misc/versionnumber.h>

#include <QAction>
#include <QDir>
#include <QFileSystemWatcher>
#include <QMessageBox>

#include <vector>

SolverDefinitionList::SolverDefinitionList(const QString& targetDir, const QLocale& locale, QObject* parent) :
	SolverDefinitionList(targetDir, "", locale, parent)
{}

SolverDefinitionList::SolverDefinitionList(const QString& targetDir, const QString& privateTargetDir, const QLocale& locale, QObject* parent) :
	QObject(parent),
	m_targetDirectory {targetDir},
	m_privateTargetDirectory {privateTargetDir},
	m_locale {locale},
	m_dialog {nullptr}
{
	// setup filesystem watcher
	QFileSystemWatcher* watcher = new QFileSystemWatcher(this);
	watcher->addPath(m_targetDirectory);
	watcher->addPath(m_privateTargetDirectory);
	connect(watcher, SIGNAL(directoryChanged(QString)), this, SLOT(updateSolverList()));
	// update solver list.
	updateSolverList();
}

SolverDefinitionList::~SolverDefinitionList()
{
	clean();
	delete m_dialog;
}

void SolverDefinitionList::updateSolverList()
{
	clean();
	QWidget* parentWidget = dynamic_cast<QWidget*>(parent());

	setupSolverList(m_targetDirectory, parentWidget);
	setupSolverList(m_privateTargetDirectory, parentWidget);

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

void SolverDefinitionList::showListDialog(QWidget* parent)
{
	if (m_dialog == nullptr) {
		m_dialog = new SolverDefinitionListDialog(this->solverList(), parent);
	}
	m_dialog->exec();
}

QString SolverDefinitionList::supportingSolverFolder(ProjectData* p, QWidget* parent)
{
	std::vector<SolverDefinitionAbstract*> compatibleSolvers;

	for (SolverDefinitionAbstract* solver : m_solverList) {
		if (solver->name() == p->mainfile()->solverName() &&
				solver->version().compatibleWith(p->mainfile()->solverVersion())) {
			compatibleSolvers.push_back(solver);
		}
	}
	if (compatibleSolvers.size() == 0) {return QString::null;}
	if (compatibleSolvers.size() == 1) {return compatibleSolvers.at(0)->absoluteFolderName();}

	SolverDefinitionListDialog dialog(compatibleSolvers, parent);
	dialog.execToSelectSolver();

	auto selectedSolver = compatibleSolvers.at(dialog.selectedSolver());
	return selectedSolver->absoluteFolderName();
}

void SolverDefinitionList::setupSolverList(const QString& dir, QWidget* w)
{
	if (dir.isEmpty()) {return;}

	QDir solversdir(dir);
	QStringList subdirs = solversdir.entryList(QStringList(), QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);

	for (const QString& subdir : subdirs) {
		QString defFileName = QDir(solversdir.absoluteFilePath(subdir)).absoluteFilePath(SolverDefinition::FILENAME);
		if (QFile::exists(defFileName)) {
			// definition.xml exists.
			try {
				SolverDefinitionAbstract* abst = new SolverDefinitionAbstract(solversdir.absoluteFilePath(subdir), m_locale, this);
				m_solverList.push_back(abst);
			} catch (ErrorMessage& e) {
				QMessageBox::warning(w, tr("Warning"), tr("Error occured while loading solver definition file in folder \"%1\". This solver is ignored.\n%2").arg(subdir).arg(e));
			}
		}
	}
}
