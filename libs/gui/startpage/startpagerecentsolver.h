#ifndef STARTPAGERECENTSOLVER_H
#define STARTPAGERECENTSOLVER_H

#include "startpagelabel.h"

class SolverDefinitionAbstract;

class StartPageRecentSolver : public StartPageLabel
{

public:
	StartPageRecentSolver(SolverDefinitionAbstract* solverDef, QWidget* parent);
	SolverDefinitionAbstract* solverDefinition() const;

private:
	SolverDefinitionAbstract* m_solverDefinition;
};

#endif // STARTPAGERECENTSOLVER_H
