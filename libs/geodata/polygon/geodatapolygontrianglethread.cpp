#define REAL double
#define VOID void

#include "geodatapolygon.h"
#include "geodatapolygonholepolygon.h"
#include "geodatapolygonregionpolygon.h"
#include "geodatapolygontrianglethread.h"

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

void freeTriangleInput(triangulateio* in)
{
	delete in->pointlist;
	delete in->segmentlist;
	delete in->regionlist;
	delete in->holelist;
}

template <typename T>
void freeMemory(T* memory)
{
	if (memory == NULL) {return;}
	trifree(memory);
}

void freeTriangleOutput(triangulateio* out)
{
	freeMemory(out->pointlist);
	freeMemory(out->trianglelist);
}

void clearTrianglateio(triangulateio* io)
{
	io->pointlist = NULL;
	io->pointattributelist = NULL;
	io->pointmarkerlist = NULL;
	io->numberofpoints = 0;
	io->numberofpointattributes = 0;
	io->trianglelist = NULL;
	io->triangleattributelist = NULL;
	io->trianglearealist = NULL;
	io->neighborlist = NULL;
	io->numberoftriangles = 0;
	io->numberofcorners = 0;
	io->numberoftriangleattributes = 0;
	io->segmentlist = NULL;
	io->segmentmarkerlist = NULL;
	io->numberofsegments = 0;
	io->holelist = NULL;
	io->numberofholes = 0;
	io->regionlist = NULL;
	io->numberofregions = 0;
	io->edgelist = NULL;
	io->edgemarkerlist = NULL;
	io->normlist = NULL;
	io->numberofedges = 0;
}

} // namespace

bool operator<(const QPointF& p1, const QPointF& p2)
{
	if (p1.x() != p2.x()) {
		return p1.x() < p2.x();
	}
	return p1.y() < p2.y();
}

GeoDataPolygonTriangleThread* GeoDataPolygonTriangleThread::m_thread = 0;

void GeoDataPolygonTriangleThread::addJob(GeoDataPolygon *polygon, bool noDraw)
{
	QMutexLocker locker(&m_mutex);
	std::list<Job>::iterator it;
	for (it = m_jobQueue.begin(); it != m_jobQueue.end(); ++it) {
		if (it->targetPolygon == polygon && it->noDraw == noDraw) {return;}
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
	if (m_thread != 0) {return m_thread;}

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

void GeoDataPolygonTriangleThread::setupTriangleInput(triangulateio* in, GeoDataPolygon* p, QPointF* offset)
{
	int pointCount = 0;
	int segmentCount = 0;
	int regionCount = 1;

	*offset = p->regionPolygon()->polygon().boundingRect().topLeft();
	geos::geom::Polygon* resultPol = getGeosPolygon(p, *offset);
	if (resultPol->isEmpty()){
		throw geos::util::GEOSException("No polygon specified");
	}

	const geos::geom::LineString* eLS = resultPol->getExteriorRing();

	pointCount += (eLS->getNumPoints() - 1);
	segmentCount += (eLS->getNumPoints() - 1);
	for (int i = 0; i < resultPol->getNumInteriorRing(); ++i){
		const geos::geom::LineString* iLS = resultPol->getInteriorRingN(i);
		pointCount += (iLS->getNumPoints() - 1);
		segmentCount += (iLS->getNumPoints() - 1);
	}
	clearTrianglateio(in);

	in->pointlist = new double[pointCount * 2];
	in->numberofpoints = pointCount;
	in->segmentlist = new int[segmentCount * 2];
	in->numberofsegments = segmentCount;
	in->holelist = new double[p->holePolygons().size() * 2];
	in->numberofholes = p->holePolygons().size();
	in->regionlist = new double[regionCount * 4];
	in->numberofregions = regionCount;

	for (int i = 0; i < eLS->getNumPoints() - 1; ++i){
		*(in->pointlist + i * 2)	   = eLS->getCoordinateN(i).x;
		*(in->pointlist + i * 2 + 1) = eLS->getCoordinateN(i).y;
		*(in->segmentlist + i * 2) = i + 1;
		*(in->segmentlist + i * 2 + 1) = (i + 1) % (eLS->getNumPoints() - 1) + 1;
	}
	geos::geom::Point* ip = resultPol->getInteriorPoint();
	QPointF innerP(ip->getX(), ip->getY());
	*(in->regionlist)	 = innerP.x();
	*(in->regionlist + 1) = innerP.y();
	*(in->regionlist + 2) = 0;
	*(in->regionlist + 3) = resultPol->getEnvelope()->getArea();
	int pOffset = 2 * (eLS->getNumPoints() - 1);
	int sOffset = (eLS->getNumPoints() - 1);
	std::vector<GeoDataPolygonAbstractPolygon*> emptyHoles;
	for (int i = 0; i < resultPol->getNumInteriorRing(); ++i){
		const geos::geom::LineString* iLS = resultPol->getInteriorRingN(i);
		for (int j = 0; j < iLS->getNumPoints() - 1; ++j){
			*(in->pointlist + j * 2 + pOffset)	   = iLS->getCoordinateN(j).x;
			*(in->pointlist + j * 2 + 1 + pOffset) = iLS->getCoordinateN(j).y;
			*(in->segmentlist + j * 2 + sOffset * 2) = j + sOffset + 1;
			*(in->segmentlist + j * 2 + 1 + sOffset * 2) = (j + 1) % (iLS->getNumPoints() - 1) + sOffset + 1;
		}
		innerP = polygonInnerPoint(p->holePolygons().at(i), emptyHoles, *offset);
		*(in->holelist + i * 2	) = innerP.x();
		*(in->holelist + i * 2 + 1) = innerP.y();
		pOffset += 2 * (iLS->getNumPoints() - 1);
		sOffset += (iLS->getNumPoints() - 1);
	}
	delete resultPol;
}

geos::geom::LinearRing* createLinearRing(GeoDataPolygonAbstractPolygon* pol, const QPointF& offset, const geos::geom::GeometryFactory* f)
{
	const geos::geom::CoordinateSequenceFactory* csf = f->getCoordinateSequenceFactory();
	QPolygonF regionPol = pol->polygon(- offset);
	geos::geom::CoordinateSequence* cs = csf->create(regionPol.size() , 2);

	std::set<QPointF> points;

	for (unsigned int i = 0; i < regionPol.size(); ++i) {
		QPointF p = regionPol.at(i);
		geos::geom::Coordinate c(p.x(), p.y(), 0);
		cs->setAt(c, i);

		if (i == regionPol.size() - 1) {continue;}

		std::set<QPointF>::const_iterator it = points.find(p);
		if (it != points.end()) {
			throw geos::util::GEOSException("Invalid ring");
		}
		points.insert(p);
	}
	return f->createLinearRing(cs);
}

geos::geom::Polygon* GeoDataPolygonTriangleThread::getGeosPolygon(GeoDataPolygon* pol, const QPointF& offset)
{
	geos::geom::LinearRing* regionRing = createLinearRing(pol->regionPolygon(), offset, m_geomFactory);
	std::vector<geos::geom::Geometry*>* holesVec = new std::vector<geos::geom::Geometry*> ();
	for (GeoDataPolygonAbstractPolygon* h : pol->holePolygons()) {
		holesVec->push_back(createLinearRing(h, offset, m_geomFactory));
	}
	return m_geomFactory->createPolygon(regionRing, holesVec);
}

QPointF GeoDataPolygonTriangleThread::polygonInnerPoint(GeoDataPolygonAbstractPolygon* region, const std::vector<GeoDataPolygonAbstractPolygon*>& holes, const QPointF& offset)
{
	geos::geom::LinearRing* regionRing = createLinearRing(region, offset, m_geomFactory);
	std::vector<geos::geom::Geometry*>* holesVec = new std::vector<geos::geom::Geometry*> ();
	for (int i = 0; i < holes.size(); ++i) {
		GeoDataPolygonAbstractPolygon* h = holes.at(i);
		holesVec->push_back(createLinearRing(h, offset, m_geomFactory));
	}
	geos::geom::Polygon* poly = m_geomFactory->createPolygon(regionRing, holesVec);
	geos::geom::Point* ip = poly->getInteriorPoint();
	QPointF ret(ip->getX(), ip->getY());
	delete poly;
	return ret;
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
	m_mutex.lock();
	GeoDataPolygon* p = m_currentJob->targetPolygon;
	p->polyData()->Reset();
	p->polyData()->Modified();

	QPointF offset;
	triangulateio in, out;

	try {
		setupTriangleInput(&in, p, &offset);
		m_mutex.unlock();
	} catch (geos::util::GEOSException&) {
		m_mutex.unlock();
		return;
	}

	char arg[] = "pQ";
	clearTrianglateio(&out);
	triangulate(&(arg[0]), &in, &out, 0);

	// free memory
	freeTriangleInput(&in);
	// copy the result to VTK containers.
	vtkPoints* points = vtkPoints::New();

	points->Allocate(out.numberofpoints);
	points->SetDataTypeToDouble();
	for (int i = 0; i < out.numberofpoints; ++i){
		double v[3];
		v[0] = *(out.pointlist + i * 2	) + offset.x();
		v[1] = *(out.pointlist + i * 2 + 1) + offset.y();
		v[2] = 0;
		points->InsertNextPoint(v);
	}
	m_isOutputting = true;

	vtkCellArray* ca = vtkCellArray::New();

	vtkIdType ids[3];
	for (int i = 0; i < out.numberoftriangles; ++i){
		for (int j = 0; j < 3; ++j) {
			ids[j] = *(out.trianglelist + i * 3 + j) - 1;
		}
		ca->InsertNextCell(3, ids);
	}
	m_mutex.lock();

	freeTriangleOutput(&out);

	m_isOutputting = false;
	if (! (m_abort || m_canceled)) {
		emit shapeUpdated(m_currentJob->targetPolygon, points, ca, m_currentJob->noDraw);
	}
	m_currentJob = nullptr;

	m_mutex.unlock();
}
