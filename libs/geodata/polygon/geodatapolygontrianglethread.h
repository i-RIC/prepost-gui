#ifndef GEODATAPOLYGONTRIANGLETHREAD_H
#define GEODATAPOLYGONTRIANGLETHREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>

#include <list>

class GeoDataPolygon;
class QPointF;
struct triangulateio;

class GeoDataPolygonTriangleThread : public QThread
{
	Q_OBJECT
public:
	void addJob(GeoDataPolygon* polygon, bool noDraw);
	void cancel(GeoDataPolygon* polygon);
	bool isOutputting(GeoDataPolygon* polygon);

	static GeoDataPolygonTriangleThread* instance();

protected:
	void run();

signals:
	void shapeUpdated(GeoDataPolygon* targetPolygon);

private:
	void runTriangle();

	QMutex m_mutex;
	QWaitCondition m_runCondition;
	bool m_canceled;

	struct Job
	{
		Job() :
			targetPolygon(0), noDraw(false)
		{}
		Job(GeoDataPolygon* p, bool nodraw) :
			targetPolygon(p), noDraw(nodraw)
		{}
		GeoDataPolygon* targetPolygon;
		bool noDraw;
	};

	std::list<Job> m_jobQueue;
	Job* m_currentJob;
	bool m_isOutputting;
	bool m_abort;

	void setupTriangleInput(triangulateio* in, GeoDataPolygon* p, QPointF* offset);

	explicit GeoDataPolygonTriangleThread();
	~GeoDataPolygonTriangleThread();

	static GeoDataPolygonTriangleThread* m_thread;
};

#endif // GEODATAPOLYGONTRIANGLETHREAD_H
