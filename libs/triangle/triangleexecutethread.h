#ifndef TRIANGLEEXECUTETHREAD_H
#define TRIANGLEEXECUTETHREAD_H

#define REAL double
#define VOID void

#include "triangle_global.h"
#include <QThread>

struct triangulateio;

class TRIANGLEDLL_EXPORT TriangleExecuteThread : public QThread
{
	Q_OBJECT
public:
	explicit TriangleExecuteThread(QObject* parent = nullptr);
	void setIOs(triangulateio* in, triangulateio* out) {
		m_in = in;
		m_out = out;
	}
	void setArgs(char* args) {
		m_args = args;
	}
	void setFileOutputSetting(QString exeFile, QString args, QString filename, QString workFolder) {
		m_exeFile = exeFile;
		m_fileArgs = args;
		m_fileName = filename;
		m_workFolder = workFolder;
	}
protected:
	void run();

private:
	triangulateio* m_in;
	triangulateio* m_out;
	char* m_args;

	QString m_exeFile;
	QString m_fileArgs;
	QString m_fileName;
	QString m_workFolder;
};

#endif // TRIANGLEEXECUTETHREAD_H
