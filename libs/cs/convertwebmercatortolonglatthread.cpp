#include "convertwebmercatortolonglatthread.h"
#include "webmercatorutil.h"

#include <QMutexLocker>

ConvertWebMercatorToLongLatThread::Job::Job() :
	rect {},
	image {},
	workDir {}
{}

ConvertWebMercatorToLongLatThread::Job::Job(const QRectF& r, const QImage& i, const QString& w) :
	rect {r},
	image {i},
	workDir {w}
{}

ConvertWebMercatorToLongLatThread::ConvertWebMercatorToLongLatThread() :
	QThread(),
	m_jobQueue {},
	m_currentJob {nullptr},
	m_abort {false}
{}

ConvertWebMercatorToLongLatThread::~ConvertWebMercatorToLongLatThread()
{
	m_abort = true;

	wait();
}

void ConvertWebMercatorToLongLatThread::addJob(const QRectF& rect, const QImage& image, const QString& workDir)
{
	QMutexLocker locker(&m_mutex);

	m_jobQueue.push_back(Job(rect, image, workDir));
}

void ConvertWebMercatorToLongLatThread::run()
{
	forever {
		if (m_abort) {return;}

		m_mutex.lock();
		Job job;
		if (m_jobQueue.size() > 0) {
			job = m_jobQueue.front();
			m_currentJob = &job;
			m_jobQueue.pop_front();
		} else {
			m_currentJob = nullptr;
		}
		m_mutex.unlock();

		if (m_currentJob == nullptr) {
			msleep(50);
			continue;
		}

		emit imageOutput(WebMercatorUtil::convertWebMercatorToLongLat(m_currentJob->rect, m_currentJob->image, m_currentJob->workDir));
	}
}
