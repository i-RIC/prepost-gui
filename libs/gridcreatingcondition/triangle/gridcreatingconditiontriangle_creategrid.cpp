#include "gridcreatingconditiontriangle.h"
#include "gridcreatingconditiontriangledivisionline.h"
#include "gridcreatingconditiontrianglegridregionpolygon.h"
#include "gridcreatingconditiontriangleholepolygon.h"
#include "gridcreatingconditiontriangleremeshpolygon.h"

#include <guibase/widget/waitdialog.h>
#include <guicore/pre/base/preprocessorgridandgridcreatingconditiondataiteminterface.h>
#include <guicore/pre/base/preprocessorgridcreatingconditiondataiteminterface.h>
#include <guicore/pre/base/preprocessorgridtypedataiteminterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <guicore/pre/grid/unstructured2dgrid.h>
#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <guicore/project/projectdata.h>
#include <misc/iricrootpath.h>
#include <misc/stringtool.h>
#include <triangle/triangleexecutethread.h>

#include <triangle/triangle.h>

#include <QApplication>
#include <QFile>
#include <QTextStream>

#include <vtkTriangle.h>

#include <geos/geom/Coordinate.h>
#include <geos/geom/CoordinateSequenceFactory.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Point.h>
#include <geos/geom/Polygon.h>
#include <geos/geom/LineString.h>
#include <geos/geom/GeometryFactory.h>

#include <map>
#include <memory>
#include <vector>

namespace {

geos::geom::Polygon* createPolygon(const QPolygonF& polygon)
{
	auto factory = geos::geom::GeometryFactory::getDefaultInstance();
	std::vector< geos::geom::Coordinate >* coordsLRing = new std::vector< geos::geom::Coordinate >();

	for (int i = 0; i < polygon.size(); i++) {
		coordsLRing->push_back(geos::geom::Coordinate(polygon[i].x(), polygon[i].y(), 0.0));
	}

	const geos::geom::CoordinateSequenceFactory* csFactory = factory->getCoordinateSequenceFactory();
	geos::geom::CoordinateSequence* csLRing = csFactory->create(coordsLRing);

	geos::geom::LinearRing* lring = factory->createLinearRing(csLRing);
	std::vector<geos::geom::Geometry*>* emptyHoles = new std::vector<geos::geom::Geometry*>();
	return factory->createPolygon(lring, emptyHoles);
}

geos::geom::LineString* createLineString(const QVector<QPointF>& polyLine)
{
	auto factory = geos::geom::GeometryFactory::getDefaultInstance();
	std::vector< geos::geom::Coordinate >* coordsLRing = new std::vector< geos::geom::Coordinate >();

	for (int i = 0; i < polyLine.size(); i++) {
		coordsLRing->push_back(geos::geom::Coordinate(polyLine[i].x(), polyLine[i].y(), 0.0));
	}

	const geos::geom::CoordinateSequenceFactory* csFactory = factory->getCoordinateSequenceFactory();
	geos::geom::CoordinateSequence* cs= csFactory->create(coordsLRing);

	return factory->createLineString(cs);
}

class PointWithId {
public:
	int id;
	double x;
	double y;
};

int findOrAddPointAndGetId(double x, double y, std::multimap<double, PointWithId>* pointmap, std::vector<double>* points)
{
	auto b = pointmap->lower_bound(x);
	auto e = pointmap->upper_bound(x);

	for (auto it = b; it != e; ++it) {
		PointWithId& p = it->second;
		if (p.x == x && p.y == y) {return p.id;}
	}
	// not found add new point
	int newid = static_cast<int>(pointmap->size()) + 1;
	pointmap->insert({x, {newid, x, y}});
	points->push_back(x);
	points->push_back(y);

	return newid;
}

void setupPointsAndSegments(geos::geom::Geometry* geom, std::vector<double>* points, std::vector<int>* segments)
{
	geos::geom::MultiLineString* strs = dynamic_cast<geos::geom::MultiLineString*> (geom);
	std::multimap<double, PointWithId> pointMap;

	for (int i = 0; i < strs->getNumGeometries(); ++i) {
		auto str = dynamic_cast<const geos::geom::LineString*> (strs->getGeometryN(i));
		geos::geom::Point* p = nullptr;
		for (int j = 0; j < str->getNumPoints() - 1; ++j) {
			p = str->getPointN(j);
			segments->push_back(findOrAddPointAndGetId(p->getX(), p->getY(), &pointMap, points));
			p = str->getPointN(j + 1);
			segments->push_back(findOrAddPointAndGetId(p->getX(), p->getY(), &pointMap, points));
		}
	}
}

void setupTriangleInput(GridCreatingConditionTriangleGridRegionPolygon* regionPolygon,
												const QList<GridCreatingConditionTriangleRemeshPolygon*>& remeshPolygons,
												const QList<GridCreatingConditionTriangleHolePolygon*>& holePolygons,
												const QList<GridCreatingConditionTriangleDivisionLine*>& divisionLines,
												bool areaConstraint, double area,
												const QPointF& offset,
												triangulateio* in)
{
	auto regionPol = std::unique_ptr<geos::geom::Polygon>(createPolygon(regionPolygon->polygon(offset)));

	std::vector<std::unique_ptr<geos::geom::Polygon> > remeshPols;
	for (GridCreatingConditionTriangleRemeshPolygon* p : remeshPolygons) {
		remeshPols.push_back(std::unique_ptr<geos::geom::Polygon> (createPolygon(p->polygon(offset))));
	}

	std::vector<std::unique_ptr<geos::geom::Polygon> > holePols;
	for (GridCreatingConditionTriangleHolePolygon* p : holePolygons) {
		holePols.push_back(std::unique_ptr<geos::geom::Polygon> (createPolygon(p->polygon(offset))));
	}

	std::vector<std::unique_ptr<geos::geom::LineString> > divLines;
	for (GridCreatingConditionTriangleDivisionLine* l : divisionLines) {
		divLines.push_back(std::unique_ptr<geos::geom::LineString> (createLineString(l->polyLine(offset))));
	}

	std::vector<geos::geom::Geometry*> lines;
	lines.push_back(const_cast<geos::geom::LineString*> (regionPol->getExteriorRing()));
	for (auto& p : remeshPols) {
		lines.push_back(const_cast<geos::geom::LineString*> (p.get()->getExteriorRing()));
	}
	for (auto& p : holePols) {
		lines.push_back(const_cast<geos::geom::LineString*> (p.get()->getExteriorRing()));
	}
	for (auto& l : divLines) {
		lines.push_back(l.get());
	}

	auto factory = geos::geom::GeometryFactory::getDefaultInstance();
	auto lineCollection = std::unique_ptr<geos::geom::GeometryCollection>
			(factory->createGeometryCollection(lines));
	auto unitedLineCollection = lineCollection->Union();

	std::vector<double> points;
	std::vector<int> segments;
	std::vector<double> holelist;
	std::vector<double> regionlist;
	setupPointsAndSegments(unitedLineCollection.get(), &points, &segments);

	in->numberofpoints = static_cast<int> (points.size() / 2);
	in->pointlist = points.data();
	in->pointattributelist = NULL;
	in->pointmarkerlist = NULL;
	in->numberofpointattributes = 0;

	in->trianglelist = NULL;
	in->triangleattributelist = NULL;
	in->trianglearealist = NULL;
	in->neighborlist = NULL;
	in->numberoftriangles = 0;
	in->numberofcorners = 0;
	in->numberoftriangleattributes = 0;

	in->numberofsegments = static_cast<int> (segments.size() / 2);
	in->segmentlist = segments.data();
	in->segmentmarkerlist = NULL;

	holelist.assign(holePolygons.size() * 2, 0);
	in->holelist = holelist.data();
	in->numberofholes = holePolygons.size();

	regionlist.assign((remeshPolygons.size() + 1) * 4, 0);
	in->regionlist = regionlist.data();
	in->numberofregions = remeshPolygons.size();

	in->edgelist = NULL;
	in->edgemarkerlist = NULL;
	in->normlist = NULL;
	in->numberofedges = 0;

	// Areas
	QPointF innerP = regionPolygon->innerPoint(offset);
	regionlist[0] = innerP.x();
	regionlist[1] = innerP.y();
	regionlist[2] = 0;
	if (areaConstraint) {
		regionlist[3] = area;
	} else {
		// no area constraint.
		auto pol = regionPolygon->polygon();
		QRectF rect = pol.boundingRect();
		regionlist[3] = rect.width() * rect.height();
	}

	for (int i = 0; i < remeshPolygons.size(); ++i) {
		GridCreatingConditionTriangleRemeshPolygon* rpol = remeshPolygons[i];
		innerP = rpol->innerPoint(offset);
		regionlist[4 * (i + 1)]     = innerP.x();
		regionlist[4 * (i + 1) + 1] = innerP.y();
		regionlist[4 * (i + 1) + 2] = 0;
		regionlist[4 * (i + 1) + 3] = rpol->cellSize();
	}

	for (int i = 0; i < holePolygons.size(); ++i) {
		GridCreatingConditionTriangleHolePolygon* hpol = holePolygons[i];
		innerP = hpol->innerPoint(offset);
		holelist[i * 2]     = innerP.x();
		holelist[i * 2 + 1] = innerP.y();
	}
}

TriangleExecuteThread* setupTriangleThread(bool angleConstraint, double angle,
																					 bool areaConstraint, bool hasRemeshPolygon,
																					 PreProcessorGridAndGridCreatingConditionDataItemInterface* dataItemInterface,
																					 ProjectData* projectData)
{
	QString args("p");

	if (angleConstraint) {
		args.append(QString("q%1").arg(angle));
	}

	if (areaConstraint || hasRemeshPolygon) {
		args.append("a");
	}

	args.append("D");

	TriangleExecuteThread* thread = new TriangleExecuteThread(nullptr);

	thread->setArgs(args);

	std::string zoneName = dataItemInterface->zoneName();

	QString triExe = iRICRootPath::get().append("/triangle.exe");
	QString workFolder = projectData->workDirectory();
	QString polyFileName = QString("%1.poly").arg(zoneName.c_str());
	QString fileArgs = "-ve";
	fileArgs.append(args);

	thread->setFileOutputSetting(triExe, fileArgs, polyFileName, workFolder);

	return thread;
}

void clearOutputIO(triangulateio* out)
{
	out->pointlist = NULL;
	out->pointattributelist = NULL;
	out->pointmarkerlist = NULL;
	out->trianglelist = NULL;
	out->triangleattributelist = NULL;
	out->trianglearealist = NULL;
	out->neighborlist = NULL;
	out->segmentlist = NULL;
	out->segmentmarkerlist = NULL;
	out->holelist = NULL;
	out->regionlist = NULL;
	out->edgelist = NULL;
	out->edgemarkerlist = NULL;
	out->normlist = NULL;
}

WaitDialog* setupWaitDialog(QWidget* parent, int progress)
{
	WaitDialog* dialog = new WaitDialog(parent);
	dialog->showProgressBar();
	dialog->setRange(0, 100);
	dialog->setUnknownLimitMode(300);
	dialog->setProgress(progress);
	dialog->setMessage(GridCreatingConditionTriangle::tr("Generating grid..."));
	return dialog;
}

void outputTriangleInputToFile(triangulateio in, const QString& filename)
{
	QFile f(filename);
	f.open(QFile::WriteOnly | QFile::Truncate | QIODevice::Text);
	QTextStream o(&f);
	o.setRealNumberPrecision(20);
	o.setRealNumberNotation(QTextStream::ScientificNotation);
	o << in.numberofpoints << " 2 0 0\n";
	for (int i = 0; i < in.numberofpoints; ++i) {
		o << i + 1 << " " << *(in.pointlist + i * 2) << " " << *(in.pointlist + i * 2 + 1) << "\n";
	}
	o << in.numberofsegments << " 0\n";
	for (int i = 0; i < in.numberofsegments; ++i) {
		o << i + 1 << " " << *(in.segmentlist + i * 2) << " " << *(in.segmentlist + i * 2 + 1) << "\n";
	}
	o << in.numberofholes << "\n";
	for (int i = 0; i < in.numberofholes; ++i) {
		o << i + 1 << " " << *(in.holelist + i * 2) << " " << *(in.holelist + i * 2 + 1) << "\n";
	}
	o << in.numberofregions << "\n";
	for (int i = 0; i < in.numberofregions; ++i) {
		o << i + 1 << " " << *(in.regionlist + i * 4) << " " << *(in.regionlist + i * 4 + 1) << " " << *(in.regionlist + i * 4 + 2) << " " << *(in.regionlist + i * 4 + 3) << "\n";
	}
	f.close();
}

void freeInputIO(triangulateio in)
{
	delete in.pointlist;
	delete in.segmentlist;
	delete in.regionlist;
	delete in.holelist;
}

void freeOutputIO(triangulateio out)
{
	if (out.pointlist != NULL) {
		trifree(out.pointlist);
	}
	if (out.trianglelist != NULL) {
		trifree(out.trianglelist);
	}
}

void setupGridFromTriangleOutput(triangulateio out, const QPointF offset, Unstructured2DGrid* grid)
{
	auto points = vtkSmartPointer<vtkPoints>::New();
	points->Allocate(out.numberofpoints);
	points->SetDataTypeToDouble();
	for (int i = 0; i < out.numberofpoints; ++i) {
		double v[3];
		v[0] = *(out.pointlist + i * 2) + offset.x();
		v[1] = *(out.pointlist + i * 2 + 1) + offset.y();
		v[2] = 0;
		points->InsertNextPoint(v);
	}
	grid->vtkGrid()->SetPoints(points);

	grid->vtkGrid()->Allocate(out.numberoftriangles);
	for (int i = 0; i < out.numberoftriangles; ++i) {
		vtkIdType id1, id2, id3;
		vtkSmartPointer<vtkTriangle> tri = vtkSmartPointer<vtkTriangle>::New();
		id1 = *(out.trianglelist + i * 3) - 1;
		id2 = *(out.trianglelist + i * 3 + 1) - 1;
		id3 = *(out.trianglelist + i * 3 + 2) - 1;
		tri->GetPointIds()->SetId(0, id1);
		tri->GetPointIds()->SetId(1, id2);
		tri->GetPointIds()->SetId(2, id3);
		grid->vtkGrid()->InsertNextCell(tri->GetCellType(), tri->GetPointIds());
	}

	// allocate memory for all grid related conditions.
	QList<GridAttributeContainer*>& clist = grid->gridAttributes();
	for (GridAttributeContainer* container: clist) {
		container->allocate();
	}
	grid->setModified();
	grid->vtkGrid()->BuildLinks();
}

} // namespace

Grid* GridCreatingConditionTriangle::createGrid()
{
	Unstructured2DGrid* grid = new Unstructured2DGrid(nullptr);
	PreProcessorGridTypeDataItemInterface* gt = dynamic_cast<PreProcessorGridTypeDataItemInterface*>(m_conditionDataItem->parent()->parent());
	gt->gridType()->buildGridAttributes(grid);

	triangulateio in, out;

	QPointF offset = m_gridRegionPolygon->polygon().boundingRect().topLeft();
	setupTriangleInput(m_gridRegionPolygon, m_remeshPolygons, m_holePolygons, m_divisionLines, m_areaConstraint, m_area, offset, &in);

	clearOutputIO(&out);

	auto di = dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItemInterface*>(m_conditionDataItem->parent());
	QString polyFileName = QString("%1.poly").arg(di->zoneName().c_str());

	TriangleExecuteThread* thread = setupTriangleThread(
				m_angleConstraint, m_angle, m_areaConstraint, m_remeshPolygons.size() > 0,
				di, projectData());
	thread->setIOs(&in, &out);

	outputTriangleInputToFile(in, QString("%1/%2").arg(projectData()->workDirectory()).arg(polyFileName));

	// start execution!
	thread->start();
	// wait for finish
	bool finished = thread->wait(2000);
	m_canceled = false;

	if (! finished) {
		int prog = 10;
		// Not finished yet. Show wait dialog.
		WaitDialog* waitDialog = setupWaitDialog(preProcessorWindow(), prog);
		connect(waitDialog, SIGNAL(canceled()), this, SLOT(cancel()));
		waitDialog->show();
		while (! finished && ! m_canceled) {
			qApp->processEvents();
			finished = thread->wait(200);
			waitDialog->setProgress(prog);
			++prog;
		}
		waitDialog->hide();
		delete waitDialog;
		if (m_canceled) {
			// not finished, but canceled;
			thread->terminate();
			thread->wait();
		}
	}
	delete thread;

	// free memory
	freeInputIO(in);

	if (m_canceled) {
		freeOutputIO(out);
		return nullptr;
	}

	setupGridFromTriangleOutput(out, offset, grid);
	freeOutputIO(out);

	return grid;
}

