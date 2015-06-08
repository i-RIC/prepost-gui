#include "startpagerecentsolver.h"

#include <guicore/solverdef/solverdefinitionabstract.h>

StartPageRecentSolver::StartPageRecentSolver(SolverDefinitionAbstract* solverDef, QWidget* parent)
	: StartPageLabel(parent)
{
	m_solverDefinition = solverDef;
	QString title("%1");
	setTitle(title.arg(solverDef->caption()));
	QString subtitle = QString("Copyright ").append(solverDef->copyright());
	setSubtitle(subtitle);
}
