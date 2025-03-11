#ifndef CONVERTWEBMERCATORTOLONGLATTHREAD_H
#define CONVERTWEBMERCATORTOLONGLATTHREAD_H

#include "cs_api.h"

#include <QMutex>
#include <QThread>

#include <list>

class QImage;

class CS_API ConvertWebMercatorToLongLatThread : public QThread
{
	Q_OBJECT

public:
	ConvertWebMercatorToLongLatThread();
	~ConvertWebMercatorToLongLatThread();

	void addJob(const QRectF& rect, const QImage& image, const QString& workDir);

signals:
	void imageOutput(const QImage& image);

private:
	void run() override;

	struct Job
	{
		Job();
		Job(const QRectF& rect, const QImage& image, const QString& workDir);

		QRectF rect;
		QImage image;
		QString workDir;
	};

	std::list<Job> m_jobQueue;
	Job* m_currentJob;
	bool m_abort;

	QMutex m_mutex;
};

#endif // CONVERTWEBMERCATORTOLONGLATTHREAD_H
