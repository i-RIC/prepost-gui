#ifndef STARTPAGEDIALOG_IMPL_H
#define STARTPAGEDIALOG_IMPL_H

#include "../startpagedialog.h"

class StartPageDialog::Impl
{
public:
	CommandMode m_commandMode;
	SolverDefinitionAbstract* m_solverDefinition;
	QString m_projectFileName;
};

#endif // STARTPAGEDIALOG_IMPL_H
