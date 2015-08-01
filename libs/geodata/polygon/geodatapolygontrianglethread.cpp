#define REAL double
#define VOID void

#include "geodatapolygon.h"
#include "geodatapolygonholepolygon.h"
#include "geodatapolygonregionpolygon.h"
#include "geodatapolygontrianglethread.h"

#include <misc/stringtool.h>
#include <triangle/triangle.h>

#include <vtkProperty.h>
#include <vtkSmartPointer.h>
#include <vtkTriangle.h>

GeoDataPolygonTriangleThread::GeoDataPolygonTriangleThread(GeoDataPolygon* parent) :
	QThread(parent)
{
	m_polygon = parent;
	m_isOutputting = false;
	m_noDraw = false;

	m_restart = false;
	m_abort = false;
	m_canceled = false;
}

GeoDataPolygonTriangleThread::~GeoDataPolygonTriangleThread()
{
	m_mutex.lock();
	m_abort = true;
	m_mutex.unlock();

	wait();
}

void GeoDataPolygonTriangleThread::update()
{
	QMutexLocker locker(&m_mutex);
	if (! isRunning()) {
		start(LowestPriority);
	} else {
		m_restart = true;
		m_runCondition.wait(&m_mutex);
	}
}

void GeoDataPolygonTriangleThread::cancel()
{
	QMutexLocker locker(&m_mutex);
	m_canceled = true;
	m_cancelCondition.wait(&m_mutex);
}

void GeoDataPolygonTriangleThread::run()
{
	forever {
		runTriangle();

		m_mutex.lock();
		if (m_canceled) {
			m_canceled = false;
			m_cancelCondition.wakeOne();
		}
		m_mutex.unlock();
		while (1) {
			QMutexLocker locker(&m_mutex);
			if (m_abort) {
				return;
			}
			if (m_canceled) {
				m_canceled = false;
				m_cancelCondition.wakeOne();
			}
			if (m_restart) {
				// restart requested!
				m_restart = false;
				break;
			}
			locker.unlock();
			// check every 10 ms.
			msleep(10);
		}
		// running started.
		m_runCondition.wakeOne();

		// when restart is requested, comes here.
		// wait for 500 ms before starting the process.
		for (int i = 0; i < 50 && ! m_canceled; ++i) {
			msleep(10);
		}
	}
}


void GeoDataPolygonTriangleThread::runTriangle()
{
	m_polygon->m_grid->Reset();
	m_polygon->m_grid->Modified();
	if (! m_polygon->checkCondition()) {
		return;
	}
	QPolygonF pol = m_polygon->m_gridRegionPolygon->polygon();
	int pointCount = 0;
	int segmentCount = 0;
	int regionCount = 1;
	pointCount += (pol.count() - 1);
	segmentCount += (pol.count() - 1);
	QPointF offset = pol.boundingRect().topLeft();
	for (int i = 0; i < m_polygon->m_holePolygons.count(); ++i) {
		pol = m_polygon->m_holePolygons[i]->polygon();
		pointCount += (pol.count() - 1);
		segmentCount += (pol.count() - 1);
	}
	triangulateio in, out;

	in.pointlist = new double[pointCount * 2];
	in.pointattributelist = NULL;
	in.pointmarkerlist = NULL;
	in.numberofpoints = pointCount;
	in.numberofpointattributes = 0;

	in.trianglelist = NULL;
	in.triangleattributelist = NULL;
	in.trianglearealist = NULL;
	in.neighborlist = NULL;
	in.numberoftriangles = 0;
	in.numberofcorners = 0;
	in.numberoftriangleattributes = 0;

	in.segmentlist = new int[segmentCount * 2];
	in.segmentmarkerlist = NULL;
	in.numberofsegments = segmentCount;

	in.holelist = new double[m_polygon->m_holePolygons.count() * 2];
	in.numberofholes = m_polygon->m_holePolygons.count();

	in.regionlist = new double[regionCount * 4];
	in.numberofregions = regionCount;

	in.edgelist = NULL;
	in.edgemarkerlist = NULL;
	in.normlist = NULL;
	in.numberofedges = 0;

	pol = m_polygon->m_gridRegionPolygon->polygon(offset);
	for (int i = 0; i < pol.count() - 1; ++i) {
		*(in.pointlist + i * 2)	 = pol.at(i).x();
		*(in.pointlist + i * 2 + 1) = pol.at(i).y();
		*(in.segmentlist + i * 2) = i + 1;
		*(in.segmentlist + i * 2 + 1) = (i + 1) % (pol.count() - 1) + 1;
	}
	QPointF innerP = m_polygon->m_gridRegionPolygon->innerPoint(offset);
	*(in.regionlist)	 = innerP.x();
	*(in.regionlist + 1) = innerP.y();
	*(in.regionlist + 2) = 0;
	QRectF rect = pol.boundingRect();
	*(in.regionlist + 3) = rect.width() * rect.height();
	int pOffset = 2 * (pol.count() - 1);
	int sOffset = (pol.count() - 1);
	for (int i = 0; i < m_polygon->m_holePolygons.count(); ++i) {
		GeoDataPolygonHolePolygon* hpol = m_polygon->m_holePolygons[i];
		pol = hpol->polygon(offset);
		for (int j = 0; j < pol.count() - 1; ++j) {
			*(in.pointlist + j * 2 + pOffset)	 = pol.at(j).x();
			*(in.pointlist + j * 2 + 1 + pOffset) = pol.at(j).y();
			*(in.segmentlist + j * 2 + sOffset * 2) = j + sOffset + 1;
			*(in.segmentlist + j * 2 + 1 + sOffset * 2) = (j + 1) % (pol.count() - 1) + sOffset + 1;
		}
		innerP = hpol->innerPoint(offset);
		*(in.holelist + i * 2) = innerP.x();
		*(in.holelist + i * 2 + 1) = innerP.y();
		pOffset += 2 * (pol.count() - 1);
		sOffset += (pol.count() - 1);
	}

	out.pointlist = NULL;
	out.pointattributelist = NULL;
	out.pointmarkerlist = NULL;
	out.trianglelist = NULL;
	out.triangleattributelist = NULL;
	out.trianglearealist = NULL;
	out.neighborlist = NULL;
	out.segmentlist = NULL;
	out.segmentmarkerlist = NULL;
	out.holelist = NULL;
	out.regionlist = NULL;
	out.edgelist = NULL;
	out.edgemarkerlist = NULL;
	out.normlist = NULL;

	QString argstr;
	argstr.append("p");
	argstr.append("D");
	char* arg = new char[argstr.length() + 1];
	strcpy(arg, iRIC::toStr(argstr).c_str());

	triangulate(arg, &in, &out, 0);

	// free memory
	delete arg;
	delete in.pointlist;
	delete in.segmentlist;
	delete in.regionlist;
	delete in.holelist;

	// copy the result to VTK containers.
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->Allocate(out.numberofpoints);
	points->SetDataTypeToDouble();
	for (int i = 0; i < out.numberofpoints; ++i) {
		double v[3];
		v[0] = *(out.pointlist + i * 2) + offset.x();
		v[1] = *(out.pointlist + i * 2 + 1) + offset.y();
		v[2] = 0;
		points->InsertNextPoint(v);
	}
	m_isOutputting = true;

	m_polygon->m_grid->SetPoints(points);
	m_polygon->m_grid->Allocate(out.numberoftriangles);
	for (int i = 0; i < out.numberoftriangles; ++i) {
		vtkIdType id1, id2, id3;
		vtkSmartPointer<vtkTriangle> tri = vtkSmartPointer<vtkTriangle>::New();
		id1 = *(out.trianglelist + i * 3) - 1;
		id2 = *(out.trianglelist + i * 3 + 1) - 1;
		id3 = *(out.trianglelist + i * 3 + 2) - 1;
		tri->GetPointIds()->SetId(0, id1);
		tri->GetPointIds()->SetId(1, id2);
		tri->GetPointIds()->SetId(2, id3);
		m_polygon->m_grid->InsertNextCell(tri->GetCellType(), tri->GetPointIds());
	}
	m_polygon->m_grid->BuildLinks();
	m_polygon->m_paintActor->GetProperty()->SetOpacity(m_polygon->m_setting.opacity);

	if (out.pointlist != NULL) {
		trifree(out.pointlist);
	}
	if (out.trianglelist != NULL) {
		trifree(out.trianglelist);
	}
	m_polygon->updateScalarValues();
	m_isOutputting = false;
	if (!(m_abort || m_restart || m_canceled) && (! m_noDraw)) {
		emit shapeUpdated();
	}
}
