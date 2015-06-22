#ifndef GEODATAPOLYGONTRIANGLETHREAD_H
#define GEODATAPOLYGONTRIANGLETHREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>

class GeoDataPolygon;

class GeoDataPolygonTriangleThread : public QThread
{
	Q_OBJECT

public:
	explicit GeoDataPolygonTriangleThread(GeoDataPolygon* parent);
	~GeoDataPolygonTriangleThread();
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

	GeoDataPolygon* m_polygon;
	bool m_restart;
	bool m_abort;
	bool m_canceled;
	bool m_isOutputting;
	bool m_noDraw;
};

#endif // GEODATAPOLYGONTRIANGLETHREAD_H
