#ifndef RAWDATAPOLYGONTRIANGLETHREAD_H
#define RAWDATAPOLYGONTRIANGLETHREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>

class RawDataPolygon;

class RawDataPolygonTriangleThread : public QThread
{
	Q_OBJECT

public:
	explicit RawDataPolygonTriangleThread(RawDataPolygon* parent);
	~RawDataPolygonTriangleThread();
	bool isOutputting() {return m_isOutputting;}
	void setNoDraw(bool nodraw) {m_noDraw = nodraw;}
	void update();
	void cancel();

protected:
	void run() override;

signals:
	void shapeUpdated();

private:
	void runTriangle();

	QMutex m_mutex;
	QWaitCondition m_runCondition;
	QWaitCondition m_cancelCondition;

	RawDataPolygon* m_polygon;
	bool m_restart;
	bool m_abort;
	bool m_canceled;
	bool m_isOutputting;
	bool m_noDraw;
};

#endif // RAWDATAPOLYGONTRIANGLETHREAD_H
