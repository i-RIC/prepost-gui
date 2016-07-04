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
	~TriangleExecuteThread();

	/// @name Functions to setup triangle running conditions
	//@{
	void setIOs(triangulateio* in, triangulateio* out);
	void setArgs(char* args);
	void setFileOutputSetting(const QString& exeFile, const QString& args, const QString& filename, const QString& workFolder);
	//@}

protected:
	void run() override;

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/triangleexecutethread_impl.h"
#endif // _DEBUG

#endif // TRIANGLEEXECUTETHREAD_H
