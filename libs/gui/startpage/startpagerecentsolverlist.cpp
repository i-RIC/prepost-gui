#include "../solverdef/solverdefinitionlist.h"
#include "startpagerecentsolver.h"
#include "startpagerecentsolverlist.h"

#include <guicore/solverdef/solverdefinitionabstract.h>

#include <QSettings>
#include <QVBoxLayout>

StartPageRecentSolverList::StartPageRecentSolverList(QWidget* parent) :
	QWidget(parent)
{
	m_layout = new QVBoxLayout(this);
	setLayout(m_layout);
	m_numberOfSolvers = 0;
}

void StartPageRecentSolverList::setSolverList(SolverDefinitionList* solverList)
{
	QMap<QString, SolverDefinitionAbstract*> sMap;
	for (SolverDefinitionAbstract* solver : solverList->solverList()) {
		sMap.insert(solver->folderName(), solver);
	}
	QSettings setting;
	QStringList recentSolvers = setting.value("general/recentsolvers", QStringList()).toStringList();
	for (const QString& solver : recentSolvers) {
		SolverDefinitionAbstract* sd = sMap.value(solver, 0);
		if (sd != 0) {add(sd);}
	}
	m_layout->addStretch(1);
}

void StartPageRecentSolverList::add(SolverDefinitionAbstract* solverDef)
{
	if (m_numberOfSolvers >= MAXSOLVERS) {return;}

	StartPageRecentSolver* s = new StartPageRecentSolver(solverDef, this);
	connect(s, SIGNAL(clicked()), this, SLOT(handleSolverSelection()));
	m_layout->addWidget(s);
	++m_numberOfSolvers;
}

void StartPageRecentSolverList::handleSolverSelection()
{
	QObject* s = sender();
	StartPageRecentSolver* solv = dynamic_cast<StartPageRecentSolver*>(s);
	emit solverSelected(solv->solverDefinition());
}
