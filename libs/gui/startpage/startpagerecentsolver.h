#ifndef STARTPAGERECENTSOLVER_H
#define STARTPAGERECENTSOLVER_H

#include "startpagelabel.h"
#include <guicore/solverdef/solverdefinitionabstract.h>
#include <QString>

class StartPageRecentSolver : public StartPageLabel
{

public:
	StartPageRecentSolver(SolverDefinitionAbstract* solverDef, QWidget* parent);
	SolverDefinitionAbstract* solverDefinition() const;

private:
	SolverDefinitionAbstract* m_solverDefinition;
};

#endif // STARTPAGERECENTSOLVER_H
