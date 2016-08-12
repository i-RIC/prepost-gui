#ifndef PROJECTWORKSPACE_IMPL_H
#define PROJECTWORKSPACE_IMPL_H

#include <QDir>

#include "../projectworkspace.h"

class ProjectWorkspace::Impl
{
public:
	Impl(ProjectWorkspace* workspace);
	void updateWorkfolderList();

	std::vector<WorkfolderInfo*> m_workfolderList;
	QDir m_workspace;
	ProjectWorkspace* m_parent;
};

#endif // PROJECTWORKSPACE_IMPL_H
