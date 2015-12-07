#ifndef TRIANGLEEXECUTETHREAD_IMPL_H
#define TRIANGLEEXECUTETHREAD_IMPL_H

#include "../triangleexecutethread.h"

#include <QString>

struct triangulateio;

class TriangleExecuteThread::Impl
{
public:
	triangulateio* m_in {nullptr};
	triangulateio* m_out {nullptr};
	char* m_args {nullptr};

	QString m_exeFile;
	QString m_fileArgs;
	QString m_fileName;
	QString m_workFolder;
};

#endif // TRIANGLEEXECUTETHREAD_IMPL_H
