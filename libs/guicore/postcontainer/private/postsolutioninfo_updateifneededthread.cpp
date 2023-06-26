#include "postsolutioninfo_updateifneededthread.h"
#include "../postbaseiterativevaluescontainer.h"

PostSolutionInfo::UpdateIfNeededThread::UpdateIfNeededThread(PostBaseIterativeValuesContainer* container) :
	QThread(),
	m_progress {0},
	m_invalidDataId {-1},
	m_container {container}
{}

int PostSolutionInfo::UpdateIfNeededThread::progress() const
{
	return m_progress;
}

int PostSolutionInfo::UpdateIfNeededThread::invalidDataId() const
{
	return m_invalidDataId;
}

void PostSolutionInfo::UpdateIfNeededThread::run()
{
	m_container->updateIfNeeded(&m_progress, &m_invalidDataId);
}
