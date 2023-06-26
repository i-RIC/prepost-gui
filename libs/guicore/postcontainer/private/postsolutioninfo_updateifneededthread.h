#ifndef POSTSOLUTIONINFO_UPDATEIFNEEDEDTHREAD_H
#define POSTSOLUTIONINFO_UPDATEIFNEEDEDTHREAD_H

#include "../postsolutioninfo.h"

#include <QThread>

class PostBaseIterativeValuesContainer;

class PostSolutionInfo::UpdateIfNeededThread : public QThread
{
public:
	UpdateIfNeededThread(PostBaseIterativeValuesContainer* container);
	int progress() const;
	int invalidDataId() const;

private:
	void run() override;

	int m_progress;
	int m_invalidDataId;

	PostBaseIterativeValuesContainer* m_container;
};

#endif // POSTSOLUTIONINFO_UPDATEIFNEEDEDTHREAD_H
