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

SolverDefinitionList::SolverDefinitionList(const QString& installDir, const QLocale& locale, QObject* parent)
	: QObject(parent)
{
	m_locale = locale;
	QString solversFolder("solvers");

	QDir inst(installDir);
	inst.cdUp();
	inst.cdUp();
	m_targetDirectory = inst.absoluteFilePath(solversFolder);
	if (! QDir(m_targetDirectory).exists()){
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

void SolverDefinitionList::updateSolverList()
{
	clean();
	QWidget* parentWidget = dynamic_cast<QWidget*>(parent());

	QDir solversdir(m_targetDirectory);
	QStringList subdirs = solversdir.entryList(QStringList(), QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);

	for (int i = 0; i < subdirs.size(); ++i){
		QString defFileName = QDir(solversdir.absoluteFilePath(subdirs.at(i))).absoluteFilePath(SolverDefinition::FILENAME);
		if (QFile::exists(defFileName)){
			// definition.xml exists.
			try {
				SolverDefinitionAbstract* abst = new SolverDefinitionAbstract(solversdir.absoluteFilePath(subdirs.at(i)), m_locale, this);
				m_solverList.append(abst);
			} catch (ErrorMessage& e){
				QMessageBox::warning(parentWidget, tr("Warning"), tr("Error occured while loading solver definition file in folder \"%1\". This solver is ignored.\n%2").arg(subdirs.at(i)).arg(e));
			}
		}
	}
	// inform the change of solverList
	emit updated();
	emit updated(this);
}

void SolverDefinitionList::clean()
{
	for (auto it = m_solverList.begin(); it != m_solverList.end(); ++it){
		delete *it;
	}
	m_solverList.clear();
}

QList<QAction*> SolverDefinitionList::actionList()
{
	QList<QAction*> ret;
	for (int i = 0; i < m_solverList.size(); ++i){
		QAction* act = new QAction(m_solverList.at(i)->caption(), this);
		ret.append(act);
	}
	return ret;
}


QString SolverDefinitionList::absoluteSolverPath(const QString& solverFolder)
{
	return QDir(m_targetDirectory).absoluteFilePath(solverFolder);
}

SolverDefinitionListDialog* SolverDefinitionList::dialog(QWidget* parent)
{
	if (m_dialog == nullptr){
		m_dialog = new SolverDefinitionListDialog(this, parent);
	}
	m_dialog->setup();
	return m_dialog;
}

const QString SolverDefinitionList::supportingSolverFolder(ProjectData* p)
{
	for (auto it = m_solverList.begin(); it != m_solverList.end(); ++it){
		SolverDefinitionAbstract* solver = *it;
		if (solver->name() == p->mainfile()->solverName() &&
				solver->version().compatibleWith(p->mainfile()->solverVersion()))
		{
			return solver->folderName();
		}
	}
	return QString::null;
}
