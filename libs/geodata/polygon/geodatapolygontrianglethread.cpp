#define REAL double
#define VOID void

#include "geodatapolygon.h"
#include "geodatapolygonholepolygon.h"
#include "geodatapolygonregionpolygon.h"
#include "geodatapolygontrianglethread.h"

#include <geoio/polygonutil.h>
#include <misc/stringtool.h>
#include <triangle/triangle.h>

#include <QMutexLocker>
#include <QPointF>

#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/CoordinateSequenceFactory.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/LinearRing.h>
#include <geos/geom/Point.h>
#include <geos/geom/Polygon.h>
#include <geos/util/GEOSException.h>

#include <vtkCellArray.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkSmartPointer.h>
#include <vtkTriangle.h>

#include <set>

// namespace for local functions
namespace {

const static int TRIANGLE_WAITTIME_MSEC = 200;

geos::geom::LinearRing* createLinearRing(GeoDataPolygonAbstractPolygon* pol, const geos::geom::GeometryFactory* f)
{
	const geos::geom::CoordinateSequenceFactory* csf = f->getCoordinateSequenceFactory();
	QPolygonF regionPol = pol->polygon();
	if (regionPol.size() < 4) {
		return nullptr;
	}

	geos::geom::CoordinateSequence* cs = csf->create(regionPol.size() , 2);

	for (int i = 0; i < regionPol.size(); ++i) {
		QPointF p = regionPol.at(i);
		geos::geom::Coordinate c(p.x(), p.y(), 0);
		cs->setAt(c, i);
	}
	return f->createLinearRing(cs);
}

} // namespace

GeoDataPolygonTriangleThread* GeoDataPolygonTriangleThread::m_thread = 0;

void GeoDataPolygonTriangleThread::addJob(GeoDataPolygon *polygon, bool noDraw)
{
	QMutexLocker locker(&m_mutex);
	for (const auto& job : m_jobQueue) {
		if (job.targetPolygon == polygon && job.noDraw == noDraw) {return;}
	}

	if (m_currentJob != nullptr && m_currentJob->targetPolygon == polygon && m_currentJob->noDraw == noDraw) {
		resetTimer();
		return;
	}
	m_jobQueue.push_back(Job(polygon, noDraw));
}

void GeoDataPolygonTriangleThread::cancel(GeoDataPolygon* polygon)
{
	QMutexLocker locker(&m_mutex);
	if (m_currentJob == 0) {return;}
	if (m_currentJob->targetPolygon == polygon) {
		m_canceled = true;
	}
}

void GeoDataPolygonTriangleThread::cancelJobs(GeoDataPolygon* polygon)
{
	cancel(polygon);
	QMutexLocker locker(&m_mutex);
	std::list<Job>::iterator it;
	for (it = m_jobQueue.begin(); it != m_jobQueue.end(); ++it) {
		if (it->targetPolygon == polygon) {
			std::list<Job>::iterator it2 = it;
			m_jobQueue.erase(it2);
			it = m_jobQueue.begin();
		}
	}
}

bool GeoDataPolygonTriangleThread::isOutputting(GeoDataPolygon *polygon)
{
	if (m_currentJob == 0) {return false;}
	return m_currentJob->targetPolygon == polygon && m_isOutputting;
}

void GeoDataPolygonTriangleThread::lockMutex()
{
	m_mutex.lock();
}

void GeoDataPolygonTriangleThread::unlockMutex()
{
	m_mutex.unlock();
}

void GeoDataPolygonTriangleThread::setLastJobDraw()
{
	QMutexLocker locker(&m_mutex);

	std::list<Job>::reverse_iterator last_it = m_jobQueue.rbegin();
	if (last_it == m_jobQueue.rend()) {return;}

	Job &lastJob = *last_it;
	lastJob.noDraw = false;
}

GeoDataPolygonTriangleThread* GeoDataPolygonTriangleThread::instance()
{
	if (m_thread != nullptr) {return m_thread;}

	m_thread = new GeoDataPolygonTriangleThread();
	return m_thread;
}

void GeoDataPolygonTriangleThread::run()
{
	forever {
		if (m_abort) {return;}

		m_mutex.lock();
		Job job;
		if (m_jobQueue.size() > 0) {
			job = m_jobQueue.front();
			m_currentJob = &job;
			m_jobQueue.pop_front();
			if (m_jobQueue.size() == 0) {
				resetTimer();
			} else {
				m_timeToStartJob = QTime::currentTime();
			}
		} else {
			m_currentJob = nullptr;
		}
		m_mutex.unlock();

		if (m_currentJob == nullptr) {
			msleep(50);
			continue;
		} else {
			QTime curr = QTime::currentTime();
			while (curr < m_timeToStartJob) {
				msleep(20);
				curr = QTime::currentTime();
			}
			runTriangle();
		}
	}
}

void GeoDataPolygonTriangleThread::resetTimer()
{
	QTime curr = QTime::currentTime();
	m_timeToStartJob = curr.addMSecs(TRIANGLE_WAITTIME_MSEC);
}

geos::geom::Polygon* GeoDataPolygonTriangleThread::getGeosPolygon(GeoDataPolygon* pol)
{
	geos::geom::LinearRing* regionRing = createLinearRing(pol->regionPolygon(), m_geomFactory);
	if (regionRing == nullptr || regionRing->getNumPoints() < 4) {
		delete regionRing;
		return nullptr;
	}
	std::vector<geos::geom::Geometry*>* holesVec = new std::vector<geos::geom::Geometry*> ();
	for (GeoDataPolygonAbstractPolygon* h : pol->holePolygons()) {
		auto holeRing = createLinearRing(h, m_geomFactory);
		if (holeRing == nullptr || holeRing->getNumPoints() < 4) {
			delete regionRing;
			for (auto h : *holesVec) {
				delete h;
			}
			return nullptr;
		}

		holesVec->push_back(holeRing);
	}
	return m_geomFactory->createPolygon(regionRing, holesVec);
}

GeoDataPolygonTriangleThread::GeoDataPolygonTriangleThread()
{
	m_currentJob = 0;
	m_abort = false;
	m_canceled = false;
	m_isOutputting = false;
	m_geomFactory = geos::geom::GeometryFactory::getDefaultInstance();

	start(LowestPriority);
}

GeoDataPolygonTriangleThread::~GeoDataPolygonTriangleThread()
{
	m_abort = true;

	wait();
}

void GeoDataPolygonTriangleThread::runTriangle()
{
	QMutexLocker locker(&m_mutex);

	auto p = getGeosPolygon(m_currentJob->targetPolygon);
	if (p == nullptr) {
		return;
	}
	std::vector<QPointF> points;
	std::vector<unsigned int> indices;

	PolygonUtil::triangulate(p, &points, &indices);

	m_isOutputting = true;

	// copy the result to VTK containers.
	auto vPoints = vtkPoints::New();

	vPoints->SetDataTypeToDouble();
	vPoints->Allocate(points.size());
	for (int i = 0; i < points.size(); ++i){
		double v[3];
		const auto& p = points.at(i);
		v[0] = p.x();
		v[1] = p.y();
		v[2] = 0;
		vPoints->InsertNextPoint(v);
	}
	vtkCellArray* ca = vtkCellArray::New();
	vtkIdType ids[3];
	for (int i = 0; i < static_cast<int> (indices.size()) / 3; ++i){
		for (int j = 0; j < 3; ++j) {
			auto pointId = indices.at(i * 3 + j);
			ids[j] = pointId;
		}
		ca->InsertNextCell(3, ids);
	}

	m_isOutputting = false;
	if (! (m_abort || m_canceled)) {
		emit shapeUpdated(m_currentJob->targetPolygon, vPoints, ca, m_currentJob->noDraw);
	}
	m_currentJob = nullptr;
}
