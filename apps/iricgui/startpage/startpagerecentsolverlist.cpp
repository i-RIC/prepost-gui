#include "../solverdef/solverdefinitionlist.h"
#include "startpagerecentsolver.h"
#include "startpagerecentsolverlist.h"

#include <QSettings>

StartPageRecentSolverList::StartPageRecentSolverList(QWidget *parent) :
	QWidget(parent)
{
	m_layout = new QVBoxLayout(this);
	setLayout(m_layout);
	m_numberOfSolvers = 0;
}

void StartPageRecentSolverList::setSolverList(SolverDefinitionList* solverList)
{
	QList<SolverDefinitionAbstract*> list = solverList->solverList();
	QList<SolverDefinitionAbstract*>::iterator it;
	QMap<QString, SolverDefinitionAbstract*> sMap;
	for (it = list.begin(); it != list.end(); ++it){
		sMap.insert((*it)->folderName(), *it);
	}
	QSettings setting;
	QStringList recentSolvers = setting.value("general/recentsolvers", QStringList()).toStringList();
	for (int i = 0; i < recentSolvers.count(); ++i){
		SolverDefinitionAbstract* sd = sMap.value(recentSolvers.at(i), 0);
		if (sd != 0){add(sd);}
	}
	m_layout->addStretch(1);
}

void StartPageRecentSolverList::add(SolverDefinitionAbstract* solverDef)
{
	if (m_numberOfSolvers >= MAXSOLVERS){return;}

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
