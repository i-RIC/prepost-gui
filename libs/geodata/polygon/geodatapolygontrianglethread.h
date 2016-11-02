#ifndef GEODATAPOLYGONTRIANGLETHREAD_H
#define GEODATAPOLYGONTRIANGLETHREAD_H

#include "gd_polygon_global.h"

#include <QMutex>
#include <QThread>
#include <QTime>

#include <list>
#include <vector>

class GeoDataPolygon;
class GeoDataPolygonAbstractPolygon;
class QPointF;

class vtkCellArray;
class vtkPoints;
struct triangulateio;

namespace geos {
namespace geom{

class Polygon;
class GeometryFactory;

} // geom
} // geos

class GD_POLYGON_EXPORT GeoDataPolygonTriangleThread : public QThread
{
	Q_OBJECT
public:
	void addJob(GeoDataPolygon* polygon, bool noDraw);
	void cancel(GeoDataPolygon* polygon);
	void cancelJobs(GeoDataPolygon* polygon);
	bool isOutputting(GeoDataPolygon* polygon);

	void lockMutex();
	void unlockMutex();

	void setLastJobDraw();
	geos::geom::Polygon* getGeosPolygon(GeoDataPolygon* pol, const QPointF& offset);

	static GeoDataPolygonTriangleThread* instance();

signals:
	void shapeUpdated(GeoDataPolygon* targetPolygon, vtkPoints* points, vtkCellArray* ca, bool noDraw);

private:
	void run();
	void runTriangle();
	void resetTimer();

	QMutex m_mutex;
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
	const geos::geom::GeometryFactory* m_geomFactory;
	QTime m_timeToStartJob;

	void setupTriangleInput(triangulateio* in, GeoDataPolygon* p, QPointF* offset);
	QPointF polygonInnerPoint(GeoDataPolygonAbstractPolygon* region, const std::vector<GeoDataPolygonAbstractPolygon*>& holes, const QPointF& offset);

	explicit GeoDataPolygonTriangleThread();
	~GeoDataPolygonTriangleThread();

	static GeoDataPolygonTriangleThread* m_thread;
};

#endif // GEODATAPOLYGONTRIANGLETHREAD_H
