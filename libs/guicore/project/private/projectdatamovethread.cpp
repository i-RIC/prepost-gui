#include "projectdatamovethread.h"

#include <misc/filesystemfunction.h>

ProjectDataMoveThread::ProjectDataMoveThread(const QString& from, const QString& to, QObject* parent) :
	QThread(parent),
	m_from {from},
	m_to {to},
	m_result {false}
{}

bool ProjectDataMoveThread::result() const
{
	return m_result;
}

void ProjectDataMoveThread::run()
{
	bool ok = iRIC::moveDirContent(m_from, m_to);
	if (! ok) {
		m_result = false;
		return;
	}

	ok = iRIC::rmdirRecursively(m_from);
	if (! ok) {
		m_result = false;
		return;
	}

	m_result = true;
}
