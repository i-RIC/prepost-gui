#include "projectdatacopythread.h"

#include <misc/filesystemfunction.h>

ProjectDataCopyThread::ProjectDataCopyThread(const QString& from, const QString& to, QObject* parent) :
	QThread(parent),
	m_from {from},
	m_to {to},
	m_result {false}
{}

bool ProjectDataCopyThread::result() const
{
	return m_result;
}

void ProjectDataCopyThread::run()
{
	m_result = iRIC::copyDirRecursively(m_from, m_to);
}
