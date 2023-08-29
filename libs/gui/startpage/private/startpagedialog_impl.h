#ifndef STARTPAGEDIALOG_IMPL_H
#define STARTPAGEDIALOG_IMPL_H

#include "../startpagedialog.h"

class StartPageDialog::Impl
{
public:
	CommandMode m_commandMode;
	SolverDefinitionAbstract* m_solverDefinition;
	QString m_projectFileName;

	iRICMainWindow* m_mainWindow;
};

#endif // STARTPAGEDIALOG_IMPL_H
