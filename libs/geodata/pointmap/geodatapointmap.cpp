#define REAL double
#define VOID void

#include "geodatapointmap.h"
#include "geodatapointmapbreakline.h"
#include "geodatapointmapdelptsgreaterthandialog.h"
#include "geodatapointmapdelptslessthandialog.h"
#include "geodatapointmapeditptsdialog.h"
#include "geodatapointmapinterpolatepointsdialog.h"
#include "geodatapointmapmergesettingdialog.h"
#include "geodatapointmaprepresentationdialog.h"
#include "private/geodatapointmap_addpointscommand.h"
#include "private/geodatapointmap_breaklineaddcommand.h"
#include "private/geodatapointmap_breaklineaddpointcommand.h"
#include "private/geodatapointmap_breaklinecanceldefinitioncommand.h"
#include "private/geodatapointmap_breaklinefinishdefinitioncommand.h"
#include "private/geodatapointmap_deletepointscommand.h"
#include "private/geodatapointmap_editpointscommand.h"
#include "private/geodatapointmap_editsinglepointcommand.h"
#include "private/geodatapointmap_interpolatelineaddpointcommand.h"
#include "private/geodatapointmap_triangleswithlongedgeremover.h"

#include <guibase/polygon/polygonpushvertexcommand.h>
#include <guibase/widget/waitdialog.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/misc/mouseboundingbox.h>
#include <misc/qundocommandhelper.h>
#include <guicore/pre/base/preprocessordataitem.h>
#include <guicore/pre/base/preprocessordatamodelinterface.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/base/preprocessorgeodatadataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <guicore/project/projectdata.h>
#include <guicore/scalarstocolors/scalarstocolorscontainer.h>
#include <misc/informationdialog.h>
#include <misc/iricundostack.h>
#include <misc/keyboardsupport.h>
#include <misc/lastiodirectory.h>
#include <misc/mathsupport.h>
#include <misc/qpointfcompare.h>
#include <misc/stringtool.h>
#include <misc/versionnumber.h>
#include <misc/zdepthrange.h>
#include <triangle/triangle.h>
#include <triangle/triangleexecutethread.h>

#include <QAction>
#include <QApplication>
#include <QDomNode>
#include <QInputDialog>
#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPolygonF>
#include <QTextStream>
#include <QTime>
#include <QToolBar>
#include <QUndoCommand>
#include <QVector2D>
#include <QVector3D>
#include <QXmlStreamWriter>

#include <vtkCellArray.h>
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkLODActor.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkTriangle.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridReader.h>
#include <vtkUnstructuredGridWriter.h>
#include <vtkVertex.h>

#include <geos/geom/Envelope.h>
#include <geos/index/quadtree/Quadtree.h>

#include <iriclib_pointmap.h>

#include <set>

namespace {

double interpolatedValue(vtkPolyData* data, vtkIdType cellId, double* weights, vtkDoubleArray* values)
{
	vtkCell* cell = data->GetCell(cellId);
	double v = 0;
	for (vtkIdType i = 0; i < cell->GetNumberOfPoints(); ++i) {
		vtkIdType vid = cell->GetPointId(i);
		v += *(weights + i) * values->GetValue(vid);
	}
	return v;
}

} // namespace

const char* GeoDataPointmap::VALUES = "values";

GeoDataPointmap::GeoDataPointmap(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* att) :
	GeoData {d, creator, att},
	m_qTree {nullptr},
	m_opacityPercent {50},
	m_representation {GeoDataPointmapRepresentationDialog::Points},
	m_addPixmap {":/libs/guibase/images/cursorAdd.png"},
	m_addCursor {m_addPixmap, 0, 0},
	m_removePixmap {":/libs/guibase/images/cursorRemove.png"},
	m_removeCursor {m_removePixmap, 0, 0},
	m_interpPointAddPixmap {":/libs/guibase/images/cursorAdd.png"},
	m_interpPointAddCursor {m_interpPointAddPixmap, 0, 0},
	m_interpPointCtrlAddPixmap {":/images/cursorCtrlAdd.png"},
	m_interpPointCtrlAddCursor {m_interpPointCtrlAddPixmap, 0, 0},
	lastInterpPointKnown {false},
	doubleclick {false},
	m_longEdgeRemover {nullptr}
{
	m_vtkGrid = vtkSmartPointer<vtkPolyData>::New();
	vtkPoints* points = vtkPoints::New();
	points->SetDataTypeToDouble();
	m_vtkGrid->SetPoints(points);
	points->Delete();

	m_selectedVerticesGrid = vtkSmartPointer<vtkPolyData>::New();
	vtkPoints* points2 = vtkPoints::New();
	points2->SetDataTypeToDouble();
	m_selectedVerticesGrid->SetPoints(points2);
	points2->Delete();

	m_vtkDelaunayedPolyData = vtkSmartPointer<vtkPolyData>::New();
	m_vtkDelaunayedPolyData->Initialize();

	m_vtkPointLocator = vtkSmartPointer<vtkPointLocator>::New();
	m_vtkPointLocator->SetDataSet(m_vtkGrid);

	setupActions();

	m_activeBreakLine = 0;
	selectionModePoint(true);

	m_pointSize = 3;
	m_needRemeshing = false;
	m_hideBreakLines = false;
}

GeoDataPointmap::~GeoDataPointmap()
{
	for (int i = 0; i < m_breakLines.count(); ++i) {
		delete m_breakLines[i];
	}

	vtkRenderer* r = renderer();
	r->RemoveActor(m_pointsActor);
	r->RemoveActor(m_actor);
	r->RemoveActor(m_selectionPolygonController.linesActor());
	r->RemoveActor(m_selectedActor);

	vtkActorCollection* col = actorCollection();
	col->RemoveAllItems();

	delete m_qTree;
}

vtkPolyData* GeoDataPointmap::vtkGrid() const
{
	return m_vtkGrid;
}

vtkPolyData* GeoDataPointmap::delaunayedPolyData() const
{
	return m_vtkDelaunayedPolyData;
}

void GeoDataPointmap::loadExternalData(const QString& filename)
{
	if (projectData()->version().build() >= 3607) {
		iRICLib::PointMap pm;
		pm.load(iRIC::toStr(filename).c_str());
		vtkPoints* points = vtkPoints::New();
		points->SetDataTypeToDouble();
		points->Allocate(pm.pointCount);

		for (int i = 0; i < pm.pointCount; ++i) {
			points->InsertNextPoint(pm.x.at(i), pm.y.at(i), 0);
		}
		points->Modified();

		vtkDataArray* da = nullptr;
		if (pm.valueType == iRICLib::PointMap::vtInt) {
			vtkIntArray* intDa = vtkIntArray::New();
			intDa->SetName(VALUES);
			for (int i = 0; i < pm.pointCount; ++i) {
				intDa->InsertNextValue(pm.intValue.at(i));
			}
			da = intDa;
		} else if (pm.valueType == iRICLib::PointMap::vtReal) {
			vtkDoubleArray* realDa = vtkDoubleArray::New();
			realDa->SetName(VALUES);
			for (int i = 0; i < pm.pointCount; ++i) {
				realDa->InsertNextValue(pm.realValue.at(i));
			}
			da = realDa;
		}

		m_vtkGrid->Initialize();
		m_vtkGrid->SetPoints(points);
		m_vtkGrid->GetPointData()->AddArray(da);
		m_vtkGrid->GetPointData()->SetActiveScalars(VALUES);
		m_vtkGrid->GetPointData()->Modified();

		vtkIdType pts[3];
		vtkCellArray* ca = vtkCellArray::New();

		for (vtkIdType i = 0; i < pm.pointCount; ++i) {
			pts[0] = i;
			ca->InsertNextCell(1, pts);
		}

		m_vtkGrid->SetVerts(ca);
		m_vtkGrid->BuildCells();
		m_vtkGrid->Modified();

		ca->Delete();

		m_vtkDelaunayedPolyData->Initialize();
		m_vtkDelaunayedPolyData->SetPoints(points);
		m_vtkDelaunayedPolyData->GetPointData()->AddArray(da);
		m_vtkDelaunayedPolyData->GetPointData()->SetActiveScalars(VALUES);
		m_vtkDelaunayedPolyData->GetPointData()->Modified();

		ca = vtkCellArray::New();
		for (unsigned int i = 0; i < pm.triangles.size(); ++i) {
			const iRICLib::PointMapTriangle& tri = pm.triangles.at(i);
			pts[0] = tri.index1 - 1;
			pts[1] = tri.index2 - 1;
			pts[2] = tri.index3 - 1;
			ca->InsertNextCell(3, pts);
		}

		m_vtkDelaunayedPolyData->SetPolys(ca);
		m_vtkDelaunayedPolyData->BuildCells();
		m_vtkDelaunayedPolyData->Modified();

		ca->Delete();
		points->Delete();
		da->Delete();

		for (unsigned int i = 0; i < pm.breaklines.size(); ++i) {
			const iRICLib::PointMapBreakline& bl = pm.breaklines.at(i);
			GeoDataPointmapBreakLine* breakline = new GeoDataPointmapBreakLine(this);
			QVector<vtkIdType> indices;
			for (unsigned int j = 0; j < bl.indices.size(); ++j) {
				indices.append(bl.indices.at(j));
			}
			breakline->setVertexIndices(indices);
			m_breakLines.append(breakline);
		}

	} else {
		// load data from the external file.
		QFileInfo finfo(filename);

		QString vtkFilename = finfo.absolutePath().append(QString("/%1.vtk").arg(finfo.baseName()));
		QFile f(vtkFilename);
		f.open(QIODevice::ReadOnly);
		QTextStream stream(&f);

		// read 4th line.
		QString line;
		for (int i = 0; i < 4; ++i) {
			line = stream.readLine();
		}
		if (line == "DATASET UNSTRUCTURED_GRID") {
			// Saved as UNSTRUCTURED GRID. backward compatibility.
			vtkUnstructuredGridReader* reader = vtkUnstructuredGridReader::New();
			reader->SetFileName(iRIC::toStr(filename).c_str());
			reader->Update();
			vtkUnstructuredGrid* grid = reader->GetOutput();
			m_vtkGrid->SetPoints(grid->GetPoints());
			m_vtkGrid->GetPointData()->AddArray(grid->GetPointData()->GetArray(VALUES));
			m_vtkGrid->GetPointData()->SetActiveScalars(VALUES);

			vtkCellArray* cells = vtkCellArray::New();
			for (vtkIdType i = 0; i < grid->GetNumberOfCells(); ++i) {
				vtkCell* cell = grid->GetCell(i);
				cells->InsertNextCell(cell->GetPointIds());
			}
			m_vtkGrid->SetVerts(cells);
			cells->Delete();
			reader->Delete();
		} else {
			// POLYDATA. new data.
			vtkPolyDataReader* reader = vtkPolyDataReader::New();
			reader->SetFileName(iRIC::toStr(vtkFilename).c_str());
			reader->Update();
			m_vtkGrid->DeepCopy(reader->GetOutput());
			reader->Delete();
		}
		m_vtkGrid->Modified();

		// load delaunayed poly data.
		QString delFilename = finfo.absolutePath().append(QString("/%1_del.vtk").arg(finfo.baseName()));
		QFile delFile(delFilename);
		if (delFile.exists()) {
			vtkPolyDataReader* reader = vtkPolyDataReader::New();
			reader->SetFileName(iRIC::toStr(delFilename).c_str());
			reader->Update();
			m_vtkDelaunayedPolyData->Reset();
			m_vtkDelaunayedPolyData->SetPoints(reader->GetOutput()->GetPoints());
			m_vtkDelaunayedPolyData->SetPolys(reader->GetOutput()->GetPolys());
			m_vtkDelaunayedPolyData->GetPointData()->AddArray(reader->GetOutput()->GetPointData()->GetArray(VALUES));
			m_vtkDelaunayedPolyData->GetPointData()->SetActiveScalars(VALUES);
			m_vtkDelaunayedPolyData->GetPointData()->Modified();
			m_vtkDelaunayedPolyData->Modified();
			m_vtkDelaunayedPolyData->BuildCells();
			reader->Delete();
		} else {
			doDelaunay();
		}

		// load break lines
		QString breaklinesFilename = finfo.absolutePath().append(QString("/%1_breaklines.dat").arg(finfo.baseName()));
		QFile breaklinesFile(breaklinesFilename);
		if (breaklinesFile.exists()) {
			breaklinesFile.open(QIODevice::ReadOnly);
			QDataStream s(&breaklinesFile);
			int num;
			s >> num;
			for (int i = 0; i < num; ++i) {
				GeoDataPointmapBreakLine* breakline = new GeoDataPointmapBreakLine(this);
				QVector<vtkIdType> indices;
				s >> indices;
				breakline->setVertexIndices(indices);
				m_breakLines.append(breakline);
			}
		}
		breaklinesFile.close();
	}

	rebuildQTree();
	updateActorSettings();
}

void GeoDataPointmap::saveExternalData(const QString& filename)
{
	iRICLib::PointMap* pm = new iRICLib::PointMap();

	vtkPoints* points = m_vtkDelaunayedPolyData->GetPoints();
	vtkDataArray* values = m_vtkDelaunayedPolyData->GetPointData()->GetArray(VALUES);

	vtkIdType pnum = points->GetNumberOfPoints();
	pm->x.reserve(pnum);
	pm->y.reserve(pnum);
	for (vtkIdType i = 0; i < pnum; ++i) {
		double point[3];
		points->GetPoint(i, point);
		pm->x.push_back(point[0]);
		pm->y.push_back(point[1]);
	}

	QString name = values->GetClassName();
	if (name == "vtkIntArray") {
		pm->valueType = iRICLib::PointMap::vtInt;
		pm->intValue.reserve(pnum);
		vtkIntArray* intValues = vtkIntArray::SafeDownCast(values);
		for (vtkIdType i = 0; i < pnum; ++i) {
			int value = intValues->GetValue(i);
			pm->intValue.push_back(value);
		}
	} else if (name == "vtkDoubleArray") {
		pm->valueType = iRICLib::PointMap::vtReal;
		pm->realValue.reserve(pnum);
		vtkDoubleArray* realValues = vtkDoubleArray::SafeDownCast(values);
		for (vtkIdType i = 0; i < pnum; ++i) {
			double value = realValues->GetValue(i);
			pm->realValue.push_back(value);
		}
	}

	vtkIdType tnum = m_vtkDelaunayedPolyData->GetNumberOfCells();
	pm->triangles.reserve(tnum);
	for (vtkIdType i = 0; i < tnum; ++i) {
		vtkCell* cell = m_vtkDelaunayedPolyData->GetCell(i);
		iRICLib::PointMapTriangle tri;
		tri.index1 = cell->GetPointId(0) + 1;
		tri.index2 = cell->GetPointId(1) + 1;
		tri.index3 = cell->GetPointId(2) + 1;
		pm->triangles.push_back(tri);
	}
	int blnum = m_breakLines.size();
	pm->breaklines.reserve(blnum);
	for (int i = 0; i < blnum; ++i) {
		GeoDataPointmapBreakLine* breakline = m_breakLines.at(i);
		const QVector<vtkIdType>& indices = breakline->vertexIndices();
		iRICLib::PointMapBreakline bl;
		bl.indices.reserve(indices.size());
		for (int j = 0; j < indices.size(); ++j) {
			bl.indices.push_back(indices.at(j));
		}
		pm->breaklines.push_back(bl);
	}

	pm->save(iRIC::toStr(filename).c_str());

	delete pm;
}

void GeoDataPointmap::updateFilename()
{
	setFilename(name().append(".dat"));
}

void GeoDataPointmap::doLoadFromProjectMainFile(const QDomNode& node)
{
	GeoData::doLoadFromProjectMainFile(node);
	int needremeshing = node.toElement().attribute("needremeshing", "0").toInt();
	m_needRemeshing = static_cast<bool>(needremeshing);
	int hidebreaklines = node.toElement().attribute("hidebreaklines", "0").toInt();
	m_hideBreakLines = static_cast<bool>(hidebreaklines);
	m_representation = (GeoDataPointmapRepresentationDialog::Representation) node.toElement().attribute("representation", "0").toInt();
	m_opacityPercent = loadOpacityPercent(node);
	m_hideBreakLines = (node.toElement().attribute("hideBreaklines", "false") == "true");
	m_pointSize = (node.toElement().attribute("pointSize", "3").toInt());
}

void GeoDataPointmap::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	GeoData::doSaveToProjectMainFile(writer);
	QString qstr;

	if (m_needRemeshing) {
		doDelaunay();
	}
	writer.writeAttribute("needremeshing", QString::number(static_cast<int>(m_needRemeshing)));
	writer.writeAttribute("hidebreaklines", QString::number(static_cast<int>(m_hideBreakLines)));
	writer.writeAttribute("representation", qstr.setNum((int) m_representation));
	writeOpacityPercent(m_opacityPercent, writer);
	if (m_hideBreakLines) {
		writer.writeAttribute("hideBreaklines", "true");
	}
	writer.writeAttribute("pointSize", qstr.setNum(m_pointSize));
}

bool GeoDataPointmap::checkBreakLines()
{
	for (int i = 0; i < m_breakLines.count(); ++i) {
		GeoDataPointmapBreakLine* line1 = m_breakLines[i];
		QVector<QPointF> l = line1->polyLine();
		for (int j = 0; j < l.count() - 1; ++j) {
			QLineF tmpline(l[j], l[j + 1]);
			for (int k = i + 1; k < m_breakLines.count(); ++k) {
				QVector<QPointF> l2 = m_breakLines[k]->polyLine();
				for (int m = 0; m < l2.count() - 1; ++m) {
					QLineF tmpline2(l2[m], l2[m + 1]);
					QPointF intersectionPoint;
					if (QLineF::BoundedIntersection == tmpline.intersect(tmpline2, &intersectionPoint)) {
						if (intersectionPoint != l2[m] && intersectionPoint != l2[m + 1]) {
							QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Break line have to have no intersection with other break lines."));
							return false;
						}
					}
				}
			}
		}
	}
	return true;
}

bool GeoDataPointmap::getValueAt(double x, double y, double* value)
{
	if (needRemeshing()) {
		remeshTINS(true);
	}

	double weights[3];
	vtkCell* cell = findCell(x, y, weights);

	if (cell == nullptr) {return false;}

	double v = 0;
	auto values = vtkDoubleArray::SafeDownCast(delaunayedPolyData()->GetPointData()->GetArray(VALUES));

	for (vtkIdType i = 0; i < cell->GetNumberOfPoints(); ++i) {
		vtkIdType vid = cell->GetPointId(i);
		v += *(weights + i) * values->GetValue(vid);
	}
	*value = v;
	return true;
}

bool GeoDataPointmap::getValueAt(const QPointF& pos, double* value)
{
	return getValueAt(pos.x(), pos.y(), value);
}

bool GeoDataPointmap::doDelaunay(bool allowCancel)
{
	bool ok = checkBreakLines();
	if (! ok) {return false;}

	triangulateio in, out;
	QTime time;
	time.start();
	int segmentCount = 0;
	for (int i = 0; i < m_breakLines.count(); ++i) {
		GeoDataPointmapBreakLine* bLine = m_breakLines[i];
		QVector<vtkIdType> indices = bLine->vertexIndices();
		if (indices.count() < 2) {continue;}
		vtkIdType prevIndex = indices[0];
		for (int j = 1; j < indices.count(); ++j) {
			vtkIdType currIndex = indices[j];
			if (prevIndex != currIndex) {++ segmentCount;}
			prevIndex = currIndex;
		}
	}
	in.pointlist = new double[m_vtkGrid->GetPoints()->GetNumberOfPoints() * 2];
	in.pointattributelist = nullptr;
	in.pointmarkerlist = nullptr;
	in.numberofpoints = m_vtkGrid->GetPoints()->GetNumberOfPoints();
	in.numberofpointattributes = 0;

	in.trianglelist = nullptr;
	in.triangleattributelist = nullptr;
	in.trianglearealist = nullptr;
	in.neighborlist = nullptr;
	in.numberoftriangles = 0;
	in.numberofcorners = 0;
	in.numberoftriangleattributes = 0;

	in.segmentlist = NULL;
	in.segmentmarkerlist = NULL;
	in.numberofsegments = 0;

	if (segmentCount > 0) {
		in.segmentlist = new int[segmentCount * 2];
		in.numberofsegments = segmentCount;
	}

	in.holelist = nullptr;
	in.numberofholes = 0;

	in.regionlist = nullptr;
	in.numberofregions = 0;

	in.edgelist = nullptr;
	in.edgemarkerlist = nullptr;
	in.normlist = nullptr;
	in.numberofedges = 0;

	for (vtkIdType i = 0; i < m_vtkGrid->GetPoints()->GetNumberOfPoints(); ++i) {
		double v[3];
		m_vtkGrid->GetPoints()->GetPoint(i, v);
		*(in.pointlist + i * 2)     = v[0];
		*(in.pointlist + i * 2 + 1) = v[1];
	}
	if (segmentCount > 0) {
		int segmentIndex = 0;
		for (int i = 0; i < m_breakLines.count(); ++i) {
			GeoDataPointmapBreakLine* bLine = m_breakLines[i];
			QVector<vtkIdType> indices = bLine->vertexIndices();
			if (indices.count() < 2) {continue;}
			vtkIdType prevIndex = indices[0];
			for (int j = 1; j < indices.count(); ++j) {
				vtkIdType currIndex = indices[j];
				if (prevIndex != currIndex) {
					*(in.segmentlist + segmentIndex * 2) = prevIndex + 1;
					*(in.segmentlist + segmentIndex * 2 + 1) = currIndex + 1;
					++ segmentIndex;
				}
				prevIndex = currIndex;
			}
		}
	}
	// code for debugging.
	/*
		QFile f("grid.poly");
		f.open(QFile::WriteOnly | QFile::Truncate | QIODevice::Text);
		QTextStream o(&f);
		o << in.numberofpoints << " 2 0 0\n";
		for (int i = 0; i < in.numberofpoints; ++i){
			o << i + 1 << " " << *(in.pointlist + i * 2) << " " << *(in.pointlist + i * 2 + 1) << "\n";
		}
		o << in.numberofsegments << " 0\n";
		for (int i = 0; i < in.numberofsegments; ++i){
			o << i + 1 << " " << *(in.segmentlist + i * 2) << " " << *(in.segmentlist + i * 2 + 1) << "\n";
		}
		o << "0\n";
		o << in.numberofregions << "\n";
		for (int i = 0; i < in.numberofregions; ++i){
			o << i + 1 << " " << *(in.regionlist + i * 4) << " " << *(in.regionlist + i * 4 + 1) << " " << *(in.regionlist + i * 4 + 2) << " " << *(in.regionlist + i * 4 + 3) << "\n";
		}
		f.close();
	*/
	out.pointlist = in.pointlist;
	out.pointattributelist = nullptr;
	out.pointmarkerlist = nullptr;
	out.trianglelist = nullptr;
	out.triangleattributelist = nullptr;
	out.trianglearealist = nullptr;
	out.neighborlist = nullptr;
	out.segmentlist = nullptr;
	out.segmentmarkerlist = nullptr;
	out.holelist = nullptr;
	out.regionlist = nullptr;
	out.edgelist = nullptr;
	out.edgemarkerlist = nullptr;
	out.normlist = nullptr;

	QString args("pcj");

	qDebug("Time for prepareing data for triangle():%d", time.elapsed());
	TriangleExecuteThread* thread = new TriangleExecuteThread(this);

	thread->setArgs(args);
	thread->setIOs(&in, &out);

	time.restart();
	// start execution!
	thread->start();

	// wait for 1 second
	bool finished = thread->wait(1000);
	m_canceled = false;

	if (! finished) {
		int prog = 10;
		// Not finished yet. Show wait dialog.
		WaitDialog waitDialog(preProcessorWindow());
		waitDialog.showProgressBar();
		waitDialog.setRange(0, 100);
		waitDialog.setUnknownLimitMode(300);
		waitDialog.setProgress(prog);
		if (! allowCancel) {
			waitDialog.disableCancelButton();
		}
		waitDialog.setMessage(tr("Remeshing TINs..."));
		connect(&waitDialog, SIGNAL(canceled()), this, SLOT(cancel()));
		waitDialog.show();
		while (! finished && ! m_canceled) {
			qApp->processEvents();
			finished = thread->wait(200);
			waitDialog.setProgress(prog);
			++prog;
		}
		waitDialog.hide();
		if (m_canceled) {
			// not finished, but canceled;
			thread->terminate();
			thread->wait();
		}
	}
	delete in.pointlist;
	if (segmentCount != 0) {
		delete in.segmentlist;
	}
	qDebug("Time for executing triangle():%d", time.elapsed());
	if (! m_canceled) {
		time.restart();
		m_vtkDelaunayedPolyData->Reset();
		m_vtkDelaunayedPolyData->Allocate(out.numberoftriangles);
		m_vtkDelaunayedPolyData->SetPoints(m_vtkGrid->GetPoints());
		qDebug("m_vtkDelaunayedPolyData Memory preparing:%d", time.elapsed());
		time.restart();
		vtkSmartPointer<vtkCellArray> ca = vtkSmartPointer<vtkCellArray>::New();
		vtkIdType pts[3];
		for (int i = 0; i < out.numberoftriangles; ++i) {
			pts[0] = *(out.trianglelist + i * 3) - 1;
			pts[1] = *(out.trianglelist + i * 3 + 1) - 1;
			pts[2] = *(out.trianglelist + i * 3 + 2) - 1;
			ca->InsertNextCell(3, pts);
		}
		m_vtkDelaunayedPolyData->SetPolys(ca);
		qDebug("m_vtkDelaunayedPolyData InsertingCells:%d", time.elapsed());
		time.restart();
		m_vtkDelaunayedPolyData->GetPointData()->Initialize();
		m_vtkDelaunayedPolyData->GetPointData()->AddArray(m_vtkGrid->GetPointData()->GetArray(VALUES));
		m_vtkDelaunayedPolyData->GetPointData()->SetActiveScalars(VALUES);
		m_vtkDelaunayedPolyData->Modified();
		m_vtkDelaunayedPolyData->GetPointData()->Modified();
		m_vtkDelaunayedPolyData->BuildCells();
		qDebug("m_vtkDelaunayedPolyData Preparing Memory:%d", time.elapsed());
		time.restart();
		qDebug("m_vtkDelaunayedPolyData Building Links:%d", time.elapsed());

		m_vtkMapper->SetInputData(m_vtkDelaunayedPolyData);

		clearPointsSelection();

		time.restart();
		m_needRemeshing = false;
		qDebug("Time for constructiong VTK Objects:%d", time.elapsed());
	}

	trifree(out.pointmarkerlist);
	trifree(out.trianglelist);
	trifree(out.segmentlist);
	trifree(out.segmentmarkerlist);

	rebuildQTree();

	return (! m_canceled);
}

void GeoDataPointmap::buildGridFromPolydata()
{
	m_vtkGrid->Reset();
	m_vtkGrid->SetPoints(m_vtkDelaunayedPolyData->GetPoints());
	m_vtkGrid->GetPointData()->Reset();
	m_vtkGrid->GetPointData()->AddArray(m_vtkDelaunayedPolyData->GetPointData()->GetArray(VALUES));
	m_vtkGrid->GetPointData()->SetActiveScalars(VALUES);
	vtkSmartPointer<vtkCellArray> ca = vtkSmartPointer<vtkCellArray>::New();
	m_vtkGrid->SetVerts(ca);
	for (int i = 0; i < m_vtkGrid->GetPoints()->GetNumberOfPoints(); ++i) {
		vtkSmartPointer<vtkVertex> vertex = vtkSmartPointer<vtkVertex>::New();
		vertex->GetPointIds()->SetId(0, i);
		m_vtkGrid->InsertNextCell(vertex->GetCellType(), vertex->GetPointIds());
	}
	m_vtkGrid->BuildCells();
	m_vtkGrid->Modified();
}

void GeoDataPointmap::setupActors()
{
	if (m_vtkMapper.Get() != nullptr) {
		// already setup.
		return;
	}

	m_vtkMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_vtkMapper->SetLookupTable(scalarsToColorsContainer()->vtkObj());
	m_vtkMapper->SetUseLookupTableScalarRange(true);
	m_vtkMapper->SetScalarVisibility(true);
	m_vtkMapper->ImmediateModeRenderingOn();

	m_actor = vtkSmartPointer<vtkLODActor>::New();
	vtkProperty* p = m_actor->GetProperty();
	p->SetRepresentationToPoints();
	p->SetLighting(false);
	m_actor->SetMapper(m_vtkMapper);
	m_actor->VisibilityOff();

	m_pointsMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_pointsMapper->SetLookupTable(scalarsToColorsContainer()->vtkObj());
	m_pointsMapper->SetUseLookupTableScalarRange(true);
	m_pointsMapper->SetScalarVisibility(true);
	m_pointsMapper->SetScalarModeToUsePointData();
	m_pointsMapper->ImmediateModeRenderingOn();

	m_pointsActor = vtkSmartPointer<vtkLODActor>::New();
	p = m_pointsActor->GetProperty();
	p->SetPointSize(m_pointSize);
	p->SetRepresentationToPoints();
	p->SetLighting(false);

	m_pointsActor->SetMapper(m_pointsMapper);
	m_actor->AddLODMapper(m_pointsMapper);

	m_maskPoints10k = vtkSmartPointer<vtkMaskPoints>::New();
	m_maskPoints10k->RandomModeOn();
	m_maskPoints10k->SetMaximumNumberOfPoints(10000);
	m_maskPoints10k->GenerateVerticesOn();
	m_maskPoints10k->SetInputData(m_vtkGrid);

	m_maskPoints40k = vtkSmartPointer<vtkMaskPoints>::New();
	m_maskPoints40k->RandomModeOn();
	m_maskPoints40k->SetMaximumNumberOfPoints(40000);
	m_maskPoints40k->GenerateVerticesOn();
	m_maskPoints40k->SetInputData(m_vtkGrid);

	m_maskPoints10kMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_maskPoints10kMapper->SetInputConnection(m_maskPoints10k->GetOutputPort());
	m_maskPoints10kMapper->ImmediateModeRenderingOn();
	m_maskPoints10kMapper->SetLookupTable(scalarsToColorsContainer()->vtkObj());
	m_maskPoints10kMapper->SetUseLookupTableScalarRange(true);
	m_maskPoints10kMapper->SetScalarVisibility(true);
	m_maskPoints10kMapper->SetScalarModeToUsePointData();

	m_maskPoints40kMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_maskPoints40kMapper->SetInputConnection(m_maskPoints40k->GetOutputPort());
	m_maskPoints40kMapper->ImmediateModeRenderingOn();
	m_maskPoints40kMapper->SetLookupTable(scalarsToColorsContainer()->vtkObj());
	m_maskPoints40kMapper->SetUseLookupTableScalarRange(true);
	m_maskPoints40kMapper->SetScalarVisibility(true);
	m_maskPoints40kMapper->SetScalarModeToUsePointData();

	m_pointsActor->AddLODMapper(m_maskPoints40kMapper);
	m_pointsActor->AddLODMapper(m_maskPoints10kMapper);

	m_actor->AddLODMapper(m_maskPoints40kMapper);
	m_actor->AddLODMapper(m_maskPoints10kMapper);

	m_selectedMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_selectedMapper->SetScalarVisibility(false);
	m_selectedMapper->SetInputData(m_selectedVerticesGrid);
	m_selectedMapper->ImmediateModeRenderingOn();

	m_selectedActor = vtkSmartPointer<vtkActor>::New();
	m_selectedActor->GetProperty()->SetPointSize(9.);
	m_selectedActor->GetProperty()->SetLighting(false);
	m_selectedActor->GetProperty()->SetColor(0, 0, 0);
	m_selectedActor->SetMapper(m_selectedMapper);
	m_selectedActor->GetProperty()->SetRepresentationToPoints();
	m_selectedActor->SetVisibility(true);

	m_selectedActor->VisibilityOn();

	m_interpolatePointsController.pointsActor()->GetProperty()->SetPointSize(7);
	m_selectionPolygonController.linesActor()->GetProperty()->SetLineWidth(2);

	vtkRenderer* r = renderer();
	r->AddActor(m_actor);
	r->AddActor(m_pointsActor);
	r->AddActor(m_selectionPolygonController.linesActor());
	r->AddActor(m_interpolatePointsController.pointsActor());
	r->AddActor(m_selectedActor);

	actorCollection()->AddItem(m_selectionPolygonController.linesActor());
	actorCollection()->AddItem(m_interpolatePointsController.pointsActor());
	actorCollection()->AddItem(m_selectedActor);

	updateActorSettings();
}

void GeoDataPointmap::setupMenu()
{
	m_menu->setTitle(tr("P&oint Cloud Data"));
	m_menu->addAction(m_editNameAction);
	m_menu->addSeparator();
	m_menu->addAction(m_selectionModePolygon);
	m_menu->addSeparator();
	m_menu->addAction(m_editPointsAction);
	m_menu->addAction(m_editPointsExportAction);
	m_menu->addSeparator();
	m_menu->addAction(m_addPointAction);
	m_menu->addAction(m_interpolatePointAction);
	m_menu->addSeparator();
	m_menu->addAction(m_editPointsDeleteAction);
	m_menu->addAction(m_editPointsLessThanAction);
	m_menu->addAction(m_editPointsGreaterThanAction);
	m_menu->addSeparator();
	m_menu->addAction(m_addBreakLineAction);
	m_menu->addAction(m_removeBreakLineAction);
	m_menu->addAction(m_removeAllBreakLinesAction);
	m_menu->addSeparator();
	m_menu->addAction(m_remeshAction);
	m_menu->addAction(m_removeTrianglesWithLongEdgeAction);
	m_menu->addSeparator();
	m_menu->addAction(m_mergeAction);
	m_menu->addSeparator();
	m_menu->addAction(m_displaySettingAction);
	m_menu->addSeparator();
	m_menu->addAction(deleteAction());

	m_rightClickingMenu = new QMenu();
	m_rightClickingMenu->addAction(m_selectionModePolygon);
	m_rightClickingMenu->addSeparator();
	m_rightClickingMenu->addAction(m_editPointsAction);
	m_rightClickingMenu->addAction(m_editPointsExportAction);
	m_rightClickingMenu->addSeparator();
	m_rightClickingMenu->addAction(m_addPointAction);
	m_rightClickingMenu->addAction(m_interpolatePointAction);
	m_rightClickingMenu->addSeparator();
	m_rightClickingMenu->addAction(m_editPointsDeleteAction);
	m_rightClickingMenu->addAction(m_editPointsLessThanAction);
	m_rightClickingMenu->addAction(m_editPointsGreaterThanAction);
	m_rightClickingMenu->addSeparator();
	m_rightClickingMenu->addAction(m_addBreakLineAction);
	m_rightClickingMenu->addAction(m_removeBreakLineAction);
	m_rightClickingMenu->addAction(m_removeAllBreakLinesAction);
	m_rightClickingMenu->addSeparator();
	m_rightClickingMenu->addAction(m_remeshAction);
	m_rightClickingMenu->addAction(m_removeTrianglesWithLongEdgeAction);
	m_rightClickingMenu->addSeparator();
	m_rightClickingMenu->addAction(m_mergeAction);
	m_rightClickingMenu->addSeparator();
	m_rightClickingMenu->addAction(m_displaySettingAction);
	m_rightClickingMenu->addSeparator();
	m_rightClickingMenu->addAction(deleteAction());
}

bool GeoDataPointmap::addToolBarButtons(QToolBar* tb)
{
	tb->addAction(m_selectionModePoint);
	tb->addAction(m_selectionModePolygon);
	tb->addSeparator();
	tb->addAction(m_addPointAction);
	tb->addAction(m_interpolatePointAction);
	tb->addSeparator();
	tb->addAction(m_addBreakLineAction);
	tb->addAction(m_removeBreakLineAction);
	return true;
}

void GeoDataPointmap::setPoints(vtkPoints* points, vtkDataArray* values)
{
	m_vtkGrid->Reset();
	m_vtkGrid->SetPoints(points);
	vtkCellArray* vArr = vtkCellArray::New();
	vArr->Allocate(points->GetNumberOfPoints());
	for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
		vtkIdType vertexId = i;
		vArr->InsertNextCell(1, &vertexId);
	}
	m_vtkGrid->SetVerts(vArr);
	m_vtkGrid->GetPointData()->Initialize();
	values->SetName(VALUES);
	m_vtkGrid->GetPointData()->AddArray(values);
	m_vtkGrid->GetPointData()->SetActiveScalars(VALUES);
	m_vtkGrid->Modified();
	m_vtkGrid->GetPoints()->Modified();
}

void GeoDataPointmap::setSTLData(vtkPolyData* data, vtkDataArray* values)
{
	m_vtkDelaunayedPolyData->DeepCopy(data);

	auto points = m_vtkDelaunayedPolyData->GetPoints();
	double v[3];
	for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
		points->GetPoint(i, v);
		v[2] = 0;
		points->SetPoint(i, v);
	}

	values->SetName(VALUES);
	m_vtkDelaunayedPolyData->GetPointData()->Reset();
	m_vtkDelaunayedPolyData->GetPointData()->AddArray(values);
	m_vtkDelaunayedPolyData->GetPointData()->SetActiveScalars(VALUES);
	m_vtkDelaunayedPolyData->BuildCells();

	buildGridFromPolydata();
}

void GeoDataPointmap::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(3);
}

void GeoDataPointmap::assignActorZValues(const ZDepthRange& range)
{
	m_zDepthRange = range;
	m_selectionPolygonController.linesActor()->SetPosition(0, 0, range.max());
	m_interpolatePointsController.pointsActor()->SetPosition(0, 0, range.max());

	double breakLineDepth = range.max() * 0.9 + range.min() * 0.1;
	for (int i = 0; i < m_breakLines.count(); ++i) {
		m_breakLines[i]->setZDepthRange(breakLineDepth, breakLineDepth);
	}
	if (m_representation == GeoDataPointmapRepresentationDialog::Points) {
		m_pointsActor->SetPosition(0, 0,   0.5 * range.max() + 0.5 * range.min());
		m_selectedActor->SetPosition(0, 0, 0.3 * range.max() + 0.7 * range.min());
	} else if (m_representation == GeoDataPointmapRepresentationDialog::Wireframe) {
		m_selectedActor->SetPosition(0, 0, 0.5 * range.max() + 0.5 * range.min());
		m_actor->SetPosition(0, 0,         0.3 * range.max() + 0.7 * range.min());
	} else if (m_representation == GeoDataPointmapRepresentationDialog::Surface) {
		m_selectedActor->SetPosition(0, 0, 0.5 * range.max() + 0.5 * range.min());
		m_actor->SetPosition(0, 0,                                   range.min());
	}
}

QDialog* GeoDataPointmap::propertyDialog(QWidget* parent)
{
	GeoDataPointmapRepresentationDialog* dialog = new GeoDataPointmapRepresentationDialog(parent);
	dialog->setRepresentation(m_representation);
	dialog->setOpacityPercent(m_opacityPercent);
	dialog->setHideBreakLines(m_hideBreakLines);
	dialog->setPointSize(m_pointSize);
	return dialog;
}

void GeoDataPointmap::handlePropertyDialogAccepted(QDialog* propDialog)
{
	GeoDataPointmapRepresentationDialog* repDialog = dynamic_cast<GeoDataPointmapRepresentationDialog*>(propDialog);
	m_representation = repDialog->representation();
	m_opacityPercent = repDialog->opacityPercent();
	m_hideBreakLines = repDialog->hideBreakLines();
	m_pointSize = repDialog->pointSize();
	if (m_representation != GeoDataPointmapRepresentationDialog::Points && m_needRemeshing) {
		int result = QMessageBox::information(preProcessorWindow(), tr("Information"), tr("When you switch to this mode, you have to remesh TINs. Do you want to remesh now?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
		if (result == QMessageBox::Yes) {
			bool ok = doDelaunay();
			if (! ok) {
				// triangulation failed.
				m_representation = GeoDataPointmapRepresentationDialog::Points;
			}
		} else {
			m_representation = GeoDataPointmapRepresentationDialog::Points;
		}
	}
	updateActorSettings();
	assignActorZValues(m_zDepthRange);
	renderGraphicsView();
}

void GeoDataPointmap::updateRepresentation()
{
	m_actor->VisibilityOff();
	m_pointsActor->VisibilityOff();
	actorCollection()->RemoveItem(m_actor);
	actorCollection()->RemoveItem(m_pointsActor);

	m_pointsActor->GetProperty()->SetPointSize(m_pointSize);
	m_actor->GetProperty()->SetPointSize(m_pointSize);
	m_selectedActor->GetProperty()->SetPointSize(m_pointSize + 2);
	switch (m_representation) {
	case GeoDataPointmapRepresentationDialog::Points:
		m_pointsMapper->SetInputData(m_vtkGrid);
		actorCollection()->AddItem(m_pointsActor);
		break;
	case GeoDataPointmapRepresentationDialog::Surface:
		m_vtkMapper->SetInputData(m_vtkDelaunayedPolyData);
		m_actor->GetProperty()->SetRepresentationToSurface();
		actorCollection()->AddItem(m_actor);
		break;
	case GeoDataPointmapRepresentationDialog::Wireframe:
		m_vtkMapper->SetInputData(m_vtkDelaunayedPolyData);
		m_actor->GetProperty()->SetRepresentationToWireframe();
		actorCollection()->AddItem(m_actor);
		break;
	}
	updateVisibilityWithoutRendering();
	assignActorZValues(m_zDepthRange);
	m_actor->GetProperty()->SetOpacity(m_opacityPercent / 100.);
}

void GeoDataPointmap::updateActorSettings()
{
	updateRepresentation();
	for (int i = 0; i < m_breakLines.count(); ++i) {
		m_breakLines[i]->setHidden(m_hideBreakLines);
	}
}

void GeoDataPointmap::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(m_editNameAction);
}

void GeoDataPointmap::showDisplaySetting()
{
	dynamic_cast<PreProcessorGeoDataDataItemInterface*>(parent())->showPropertyDialog();
}

void GeoDataPointmap::setupActions()
{
	m_addPointAction = new QAction(tr("&Add New Point"), this);
	m_addPointAction->setIcon(QIcon(":/libs/geodata/pointmap/images/iconAddPoint.png"));
	m_addPointAction->setCheckable(true);
	connect(m_addPointAction, SIGNAL(triggered(bool)), this, SLOT(addPoints(bool)));

	m_interpolatePointAction = new QAction(tr("&Interpolate Points"), this);
	m_interpolatePointAction->setIcon(QIcon(":/libs/geodata/pointmap/images/iconInterpolatePoint.png"));
	m_interpolatePointAction->setCheckable(true);
	connect(m_interpolatePointAction, SIGNAL(triggered(bool)), this, SLOT(interpolatePoints(bool)));

	m_displaySettingAction = new QAction(tr("Display &Setting..."), this);
	connect(m_displaySettingAction, SIGNAL(triggered()), this, SLOT(showDisplaySetting()));

	m_selectionModePoint = new QAction(QIcon(":/libs/geodata/pointmap/images/iconPointSelection.png"), tr("Select point"), this);
	connect(m_selectionModePoint, SIGNAL(triggered(bool)), this, SLOT(selectionModePoint(bool)));
	m_selectionModePoint->setCheckable(true);

	m_selectionModePolygon = new QAction(tr("Select Points With &Polygon"), this);
	m_selectionModePolygon->setIcon(QIcon(":/libs/geodata/pointmap/images/iconPolygonSelection.png"));
	m_selectionModePolygon->setCheckable(true);
	connect(m_selectionModePolygon, SIGNAL(triggered(bool)), this, SLOT(selectionModePolygon(bool)));

	m_editPointsAction = new QAction(tr("&Edit Selected Points..."), this);
	m_editPointsAction->setCheckable(false);
	connect(m_editPointsAction, SIGNAL(triggered(bool)), this, SLOT(editPoints()));

	m_editPointsDeleteAction = new QAction(tr("Delete Selected P&oints"), this);
	m_editPointsDeleteAction->setCheckable(false);
	connect(m_editPointsDeleteAction, SIGNAL(triggered(bool)), this, SLOT(editPointsDelete()));

	m_editPointsExportAction = new QAction(tr("E&xport Selected Points..."), this);
	m_editPointsExportAction->setCheckable(false);
	connect(m_editPointsExportAction, SIGNAL(triggered(bool)), this, SLOT(editPointsExport()));

	m_editPointsLessThanAction = new QAction(tr("Delete Selected Points &Less Than Value..."), this);
	m_editPointsLessThanAction->setCheckable(false);
	connect(m_editPointsLessThanAction, SIGNAL(triggered(bool)), this, SLOT(editPointsLessThan()));

	m_editPointsGreaterThanAction = new QAction(tr("Delete Selected Points &Greater Than Value..."), this);
	m_editPointsGreaterThanAction->setCheckable(false);
	connect(m_editPointsGreaterThanAction, SIGNAL(triggered(bool)), this, SLOT(editPointsGreaterThan()));

	enablePointSelectedActions(false);

	m_remeshAction = new QAction(tr("Remesh &TINs"), this);
	connect(m_remeshAction, SIGNAL(triggered()), this, SLOT(remeshTINS()));

	m_addBreakLineAction = new QAction(tr("Add &Break Line"), this);
	m_addBreakLineAction->setIcon(QIcon(":/libs/geodata/pointmap/images/iconBreakLineAdd.png"));
	m_addBreakLineAction->setCheckable(true);
	connect(m_addBreakLineAction, SIGNAL(triggered()), this, SLOT(addBreakLine()));

	m_removeBreakLineAction = new QAction(tr("&Remove Break Line"), this);
	m_removeBreakLineAction->setIcon(QIcon(":/libs/geodata/pointmap/images/iconBreakLineDelete.png"));
	m_removeBreakLineAction->setCheckable(true);
	connect(m_removeBreakLineAction, SIGNAL(triggered()), this, SLOT(removeBreakLine()));

	m_removeAllBreakLinesAction = new QAction(tr("Re&move All Break Lines..."), this);
	connect(m_removeAllBreakLinesAction, SIGNAL(triggered()), this, SLOT(removeAllBreakLines()));

	m_removeTrianglesWithLongEdgeAction = new QAction(tr("Remove Triangles &with Long edge..."), this);
	connect(m_removeTrianglesWithLongEdgeAction, SIGNAL(triggered()), this, SLOT(removeTrianglesWithLongEdgeStart()));

	m_mergeAction = new QAction(tr("Merge..."), this);
	connect(m_mergeAction, SIGNAL(triggered()), this, SLOT(mergePointmaps()));
}

void GeoDataPointmap::mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (event->button() == Qt::LeftButton) {
		// left click
		double worldX = static_cast<double>(event->x());
		double worldY = static_cast<double>(event->y());
		v->viewportToWorld(worldX, worldY);
		auto box = dataModel()->mouseBoundingBox();

		switch (m_mouseEventMode) {
		case meNormal:
		case meNormalWithShift:
			box->setStartPoint(event->x(), event->y());
			box->enable();
			v->GetRenderWindow()->SetDesiredUpdateRate(PreProcessorDataItem::dragUpdateRate);
			break;
		case meSMPolygonPrepare:
		case meSMPolygon:
			m_mouseEventMode = meSMPolygon;
			pushRenderCommand(new PolygonPushVertexCommand(true, v->viewportToWorld(event->pos()), &m_selectionPolygonController));
			break;
		case meSMInterpPointPrepare: {
				double p[3];
				m_vtkGrid->GetPoint(m_selectedVertexId, p);
				pushRenderCommand(new InterpolateLineAddPointCommand(p[0], p[1], m_selectedZPos, false, this));
				m_mouseEventMode = meSMInterpPoint;
			}
			break;
		case meSMInterpPoint:
			break;
		case meSMInterpCtrlPoint:
			pushRenderCommand(new InterpolateLineAddPointCommand(worldX, worldY, 0, true, this));
			break;
		case meSMAddPoint:
			break;
		case meSMAddCtrlPoint:
			if (m_interpolatePointsController.polyLine().size() > 0) {
				pushRenderCommand(new InterpolateLineAddPointCommand(worldX, worldY, 0, true, this));
			}
			break;
		case meSMAddPointPrepare:
			if (m_interpolatePointsController.polyLine().size() == 0) {
				double p[3];
				m_vtkGrid->GetPoint(m_selectedVertexId, p);
				m_addPointsValue = m_selectedZPos;
				pushRenderCommand(new InterpolateLineAddPointCommand(p[0], p[1], 0, false, this));
				m_mouseEventMode = meSMAddCtrlPoint;
			}
			break;
		case meBreakLineAddNotPossible:
			// do nothing.
			break;

		case meBreakLineAdd:
			// add point.
			pushRenderCommand(new BreakLineAddPointCommand(false, m_selectedVertexId, this));
			break;
		case meBreakLineRemoveNotPossible:
			// do nothing.
			break;
		case meBreakLineRemove:
			// remove active break line.
			m_activeBreakLine->setActive(true);
			if (QMessageBox::No == QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Are you sure you want to remove this break line?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No)) {
				m_activeBreakLine->setActive(false);
				return;
			}
			delete m_activeBreakLine;
			m_breakLines.removeOne(m_activeBreakLine);
			m_activeBreakLine = 0;
			m_mouseEventMode = meNormal;
			iRICUndoStack::instance().clear();
			updateActionStatus();
			break;
		default:
			break;
		}
		updateMouseCursor(v);
	} else if (event->button() == Qt::RightButton) {
		// right click
		m_dragStartPoint = QPoint(event->x(), event->y());
	}
	renderGraphicsView();
}
void GeoDataPointmap::mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (doubleclick) {
		doubleclick = false;
		return;
	}
	if (event->button() == Qt::LeftButton) {
		// left click
		double worldX = static_cast<double>(event->x());
		double worldY = static_cast<double>(event->y());
		v->viewportToWorld(worldX, worldY);
		MouseBoundingBox* box = dataModel()->mouseBoundingBox();
		bool xOr = ((event->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier);

		switch (m_mouseEventMode) {
		case meNormal:
		case meNormalWithShift:
			box->setEndPoint(event->x(), event->y());
			if (iRIC::isNear(box->startPoint(), box->endPoint())) {
				selectPointsNearPoint(QVector2D(worldX, worldY), xOr);
			} else {
				selectPointsInsideBox(box, xOr);
			}
			box->disable();
			v->restoreUpdateRate();
			renderGraphicsView();
			break;
		default:
			break;
		}
	} else if (event->button() == Qt::RightButton) {
		if (iRIC::isNear(m_dragStartPoint, event->pos())) {
			// show right-clicking menu.
			m_rightClickingMenu->move(event->globalPos());
			m_rightClickingMenu->show();
		}
	}
}

void GeoDataPointmap::mouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	m_mouseMovePoint = QPoint(event->x(), event->y());
	MouseBoundingBox* box = dataModel()->mouseBoundingBox();

	switch (m_mouseEventMode) {
	case meNormal:
	case meNormalWithShift:
		box->setEndPoint(event->x(), event->y());
		renderGraphicsView();
		break;
	case meSMPolygon:
		pushRenderCommand(new PolygonPushVertexCommand(false, v->viewportToWorld(event->pos()), &m_selectionPolygonController));
		break;
	case meSMInterpPoint:
	case meSMInterpPointPrepare:
	case meSMInterpCtrlPoint:
	case meSMAddPoint:
	case meSMAddPointPrepare:
	case meSMAddCtrlPoint:
	case meBreakLineAdd:
	case meBreakLineAddNotPossible:
	case meBreakLineRemove:
	case meBreakLineRemoveNotPossible:
		updateMouseEventMode();
		updateMouseCursor(v);
		break;
	default:
		break;
	}
}
void GeoDataPointmap::mouseDoubleClickEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	doubleclick = true;
	bool xOr = ((event->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier);

	if (m_mouseEventMode == meSMPolygon) {
		definePolygon(true, xOr);

	} else if (m_mouseEventMode == meSMAddCtrlPoint) {
		finishAddPoint();
	} else if (m_mouseEventMode == meSMInterpPointPrepare || m_mouseEventMode == meSMInterpPoint) {
		finishInterpPoint();
	} else if (m_mouseEventMode == meBreakLineAdd || m_mouseEventMode == meBreakLineAddNotPossible) {
		pushRenderCommand(new BreakLineFinishDefinitionCommand(this));
	}
	updateMouseCursor(v);
}
void GeoDataPointmap::keyPressEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* v)
{
	bool xOr = ((event->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier);
	switch (m_mouseEventMode) {
	case meNormal:
	case meNormalWithShift:
		m_mouseEventMode = meNormal;
		if (xOr) {
			// shift pressed.
			m_mouseEventMode = meNormalWithShift;
		}
		updateMouseCursor(graphicsView());
		break;
	case meSMPolygon:
		if (iRIC::isEnterKey(event->key())) {
			definePolygon(false, xOr);
		}
		break;
	default:
		break;
	}

	if (iRIC::isEnterKey(event->key())) {
		if (m_mouseEventMode == meSMPolygon) {
			definePolygon(false, xOr);
		} else if (m_mouseEventMode == meSMAddCtrlPoint) {
			finishAddPoint();
		} else if (m_mouseEventMode == meSMInterpPointPrepare || m_mouseEventMode == meSMInterpPoint) {
			finishInterpPoint();
		} else if (m_mouseEventMode == meBreakLineAdd || m_mouseEventMode == meBreakLineAddNotPossible) {
			pushRenderCommand(new BreakLineFinishDefinitionCommand(this));
		}
	} else if (event->key() == Qt::Key_Alt) {
		if (m_mouseEventMode == meSMInterpPointPrepare) {
			m_mouseEventMode = meSMInterpCtrlPoint;
		} else if (m_mouseEventMode == meSMInterpPoint) {
			m_mouseEventMode = meSMInterpCtrlPoint;
		}
	}
	updateMouseCursor(v);
}
void GeoDataPointmap::keyReleaseEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* v)
{
	bool xOr = ((event->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier);
	switch (m_mouseEventMode) {
	case meNormal:
	case meNormalWithShift:
		m_mouseEventMode = meNormal;
		if (xOr) {
			// shift pressed.
			m_mouseEventMode = meNormalWithShift;
		}
		updateMouseCursor(graphicsView());
		break;
	case meSMInterpCtrlPoint:
		if (event->key() == Qt::Key_Control) {
			m_mouseEventMode = meSMInterpPoint;
		} else if (event->key() == Qt::Key_Alt) {
			m_mouseEventMode = meSMInterpPoint;
		}
		break;
	default:
		break;
	}
	updateMouseCursor(v);
}

void GeoDataPointmap::definePolygon(bool doubleClick, bool xOr)
{
	int minCount = 4;
	if (doubleClick) {minCount = 3;}
	if (m_selectionPolygonController.polygon().count() <= minCount) {
		QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Polygon must have three vertices at least."));
		return;
	}
	selectPointsInsidePolygon(xOr);
	m_selectionPolygonController.clear();
	m_mouseEventMode = GeoDataPointmap::meNormal;
	updateActionStatus();
}

void GeoDataPointmap::selectPointsInsideBox(MouseBoundingBox* box, bool xOr)
{
	vtkTriangle* tri = vtkTriangle::New();
	double center[3], tmpp[3];
	center[0] = 0; center[1] = 0; center[2] = 0;
	vtkIdType pnum = box->vtkGrid()->GetNumberOfPoints();
	for (vtkIdType i = 0; i < pnum; ++i) {
		box->vtkGrid()->GetPoint(i, tmpp);
		center[0] += tmpp[0];
		center[1] += tmpp[1];
	}
	center[0] /= pnum;
	center[1] /= pnum;
	QVector2D tmpv(tmpp[0] - center[0], tmpp[1] - center[1]);
	double radius = tmpv.length() * 1.1;
	vtkSmartPointer<vtkIdList> idlist = vtkSmartPointer<vtkIdList>::New();
	m_vtkPointLocator->FindPointsWithinRadius(radius, center, idlist);

	vtkSmartPointer<vtkUnstructuredGrid> tmpg = vtkSmartPointer<vtkUnstructuredGrid>::New();
	tmpg->Allocate(2, 1);
	tri->GetPointIds()->SetId(0, 0);
	tri->GetPointIds()->SetId(1, 1);
	tri->GetPointIds()->SetId(2, 2);
	tmpg->InsertNextCell(tri->GetCellType(), tri->GetPointIds());
	tri->GetPointIds()->SetId(0, 2);
	tri->GetPointIds()->SetId(1, 3);
	tri->GetPointIds()->SetId(2, 0);
	tmpg->InsertNextCell(tri->GetCellType(), tri->GetPointIds());
	tmpg->SetPoints(box->vtkGrid()->GetPoints());
	tri->Delete();
	QSet<vtkIdType> oldSelectedVertices;
	if (xOr) {
		vtkCellArray* cArr = m_selectedVerticesGrid->GetVerts();
		cArr->InitTraversal();
		for (vtkIdType i = 0; i < cArr->GetNumberOfCells(); ++i) {
			vtkIdType numPoints;
			vtkIdType* points;
			cArr->GetNextCell(numPoints, points);
			oldSelectedVertices.insert(*(points));
		}
	}
	m_selectedVerticesGrid->Reset();
	m_selectedVerticesGrid->Allocate(m_vtkGrid->GetNumberOfPoints() / 10 + 1);
	m_selectedVerticesGrid->SetPoints(m_vtkGrid->GetPoints());

	QVector<vtkIdType> newSelectedVertices;
	vtkSmartPointer<vtkVertex> v = vtkSmartPointer<vtkVertex>::New();
//	qDebug("selectPointsInsideBox start");
	for (vtkIdType i = 0; i < idlist->GetNumberOfIds(); i++) {
		double p[3];
		double weights[3];
		vtkCell* hintCell = 0;
		double pcoords[3];
		int subid;
		m_vtkGrid->GetPoints()->GetPoint(idlist->GetId(i) , p);
		vtkIdType cellid = tmpg->FindCell(p, hintCell, 0, 1e-4, subid, pcoords, weights);
		if (cellid >= 0) {
			// this is inside the box!
			vtkIdType pointId = idlist->GetId(i);
			if (oldSelectedVertices.contains(pointId)) {
				// this should not be included.
				oldSelectedVertices.remove(pointId);
			} else {
				// add this point.
				newSelectedVertices.push_back(pointId);
			}
		}
	}
	// points included in oldSelectedVertices should be added.
	for (auto it = oldSelectedVertices.begin(); it != oldSelectedVertices.end(); ++it) {
		vtkIdType pointId = *it;
		newSelectedVertices.append(pointId);
	}
	// quick sort, and add.
//	qSort(newSelectedVertices);
	vtkCellArray* vArr = vtkCellArray::New();
	vArr->Allocate(newSelectedVertices.count(), 10);
	for (int i = 0; i < newSelectedVertices.count(); ++i) {
		vtkIdType vertexId = newSelectedVertices[i];
		vArr->InsertNextCell(1, &vertexId);
//		qDebug(" Index %d", vertexId);
	}
	m_selectedVerticesGrid->SetVerts(vArr);
//	qDebug("selectPointsInsideBox end");

	m_selectedVerticesGrid->Modified();
	enablePointSelectedActions(vArr->GetNumberOfCells() > 0);
	vArr->Delete();
}

void GeoDataPointmap::selectPointsNearPoint(const QVector2D& pos, bool xOr)
{
	double point[3] = {pos.x(), pos.y(), 0};
	vtkIdType nearestId = m_vtkPointLocator->FindClosestPoint(point);
	double point2[3];
	m_vtkGrid->GetPoint(nearestId, point2);
	QVector2D vertexPos(point2[0], point2[1]);
	double limitdist = graphicsView()->stdRadius(iRIC::nearRadius());
	double dist = (vertexPos - pos).length();
	vtkIdType pointToAdd = -1;
	if (dist < limitdist) {
		// it is near enough. select this point.
		pointToAdd = nearestId;
	}
	QSet<vtkIdType> oldSelectedVertices;
	if (xOr) {
		vtkCellArray* cArr = m_selectedVerticesGrid->GetVerts();
		cArr->InitTraversal();
		for (vtkIdType i = 0; i < cArr->GetNumberOfCells(); ++i) {
			vtkIdType numPoints;
			vtkIdType* points;
			cArr->GetNextCell(numPoints, points);
			oldSelectedVertices.insert(*(points));
		}
	}
	m_selectedVerticesGrid->Initialize();
	if (xOr) {
		m_selectedVerticesGrid->Allocate(oldSelectedVertices.count(), 10);
	} else {
		m_selectedVerticesGrid->Allocate(3, 3);
	}
	m_selectedVerticesGrid->SetPoints(m_vtkGrid->GetPoints());
	if (pointToAdd != -1) {
		if (oldSelectedVertices.contains(pointToAdd)) {
			oldSelectedVertices.remove(pointToAdd);
		} else {
			oldSelectedVertices.insert(pointToAdd);
		}
	}
	vtkCellArray* vArr = vtkCellArray::New();
	vArr->Allocate(oldSelectedVertices.count(), 10);
	for (auto it = oldSelectedVertices.begin(); it != oldSelectedVertices.end(); ++it) {
		vtkIdType vertexId = *it;
		vArr->InsertNextCell(1, &vertexId);
	}
	m_selectedVerticesGrid->SetVerts(vArr);
	m_selectedVerticesGrid->Modified();
	enablePointSelectedActions(vArr->GetNumberOfCells() > 0);
	vArr->Delete();
}

void GeoDataPointmap::selectPointsInsidePolygon(bool xOr)
{

	QSet<vtkIdType> oldSelectedVertices;
	if (xOr) {
		for (vtkIdType j = 0; j < m_selectedVerticesGrid->GetNumberOfCells(); ++j) {
			vtkCell* cell = m_selectedVerticesGrid->GetCell(j);
			vtkIdType pointId = cell->GetPointId(0);
			oldSelectedVertices.insert(pointId);
		}
	}
	m_selectedVerticesGrid->Reset();
	m_selectedVerticesGrid->SetPoints(m_vtkGrid->GetPoints());

	for (int i = 0; i < m_selectedVerticesGrid->GetNumberOfPoints(); i++) {
		double x[3];
		m_selectedVerticesGrid->GetPoint(i, x);
		bool inside = m_selectionPolygonController.isAreaSelectable(QPointF(x[0], x[1]));
		if (inside || oldSelectedVertices.contains(i)) {
			vtkSmartPointer<vtkVertex> v = vtkSmartPointer<vtkVertex>::New();
			v->GetPointIds()->SetId(0, i);
			m_selectedVerticesGrid->InsertNextCell(v->GetCellType(), v->GetPointIds());
		}
	}
	m_selectedVerticesGrid->Modified();
	enablePointSelectedActions(true);
}
void GeoDataPointmap::selectionModePoint(bool on)
{
	if (on) {
		m_mouseEventMode = meNormal;
	} else {
		m_mouseEventMode = meNormal;
	}
	updateActionStatus();
}

void GeoDataPointmap::selectionModePolygon(bool on)
{
	clearPointsSelection();

	if (on) {
		m_mouseEventMode = meSMPolygonPrepare;
	} else {
		m_mouseEventMode = meNormal;
	}
	updateActionStatus();
}

void GeoDataPointmap::updateActionStatus()
{
	m_selectionModePoint->setDisabled(true);
	m_selectionModePoint->setChecked(false);

	m_selectionModePolygon->setDisabled(true);
	m_selectionModePolygon->setChecked(false);

	m_addPointAction->setDisabled(true);
	m_addPointAction->setChecked(false);

	m_interpolatePointAction->setDisabled(true);
	m_interpolatePointAction->setChecked(false);

	m_addBreakLineAction->setDisabled(true);
	m_addBreakLineAction->setChecked(false);

	m_removeBreakLineAction->setDisabled(true);
	m_removeBreakLineAction->setChecked(false);

	m_removeAllBreakLinesAction->setDisabled(true);
	m_removeAllBreakLinesAction->setChecked(false);

	switch (m_mouseEventMode) {
	case meNormal:
	case meNormalWithShift:
		m_selectionModePoint->setEnabled(true);
		m_selectionModePoint->setChecked(true);

		m_selectionModePolygon->setEnabled(true);
		m_addPointAction->setEnabled(true);
		m_interpolatePointAction->setEnabled(true);
		m_addBreakLineAction->setEnabled(true);
		m_addBreakLineAction->setEnabled(true);
		m_removeBreakLineAction->setEnabled(true);
		m_removeAllBreakLinesAction->setEnabled(true);
		break;
	case meSMPolygonPrepare:
	case meSMPolygon:
		m_selectionModePoint->setEnabled(false);

		m_selectionModePolygon->setEnabled(true);
		m_selectionModePolygon->setChecked(true);
		break;
	case meSMInterpPoint:
		m_interpolatePointAction->setEnabled(true);
		m_interpolatePointAction->setChecked(true);
		break;
	case meSMAddPoint:
		m_addPointAction->setEnabled(true);
		m_addPointAction->setChecked(true);
		break;
	case meBreakLineAdd:
	case meBreakLineAddNotPossible:
		m_addBreakLineAction->setEnabled(true);
		m_addBreakLineAction->setChecked(true);
		break;
	case meBreakLineRemove:
	case meBreakLineRemoveNotPossible:
		m_removeBreakLineAction->setEnabled(true);
		m_removeBreakLineAction->setChecked(true);
		break;
	default:
		break;
	}
}

void GeoDataPointmap::updateMouseEventMode()
{
	double dx, dy;
	dx = m_mouseMovePoint.x();
	dy = m_mouseMovePoint.y();
	graphicsView()->viewportToWorld(dx, dy);
	QPointF worldPos(dx, dy);
	double tmppos[3] = {dx, dy, 0};
	switch (m_mouseEventMode) {
	case meNormal:
	case meNormalWithShift:
		// do nothing.
		break;
	case meSMInterpPoint:
		if (isVertexSelectable(worldPos)) {
			m_mouseEventMode = meSMInterpPointPrepare;
		} else {
			m_mouseEventMode = meSMInterpPoint;
		}
		break;
	case meSMInterpPointPrepare:
		if (isVertexSelectable(worldPos)) {
			m_mouseEventMode = meSMInterpPointPrepare;
		} else {
			m_mouseEventMode = meSMInterpPoint;
		}
		break;
	case meSMInterpCtrlPoint:
		//if (isVertexSelectable(worldPos)){
		//	m_mouseEventMode = meSMInterpPoint;
		//}else{
		//	m_mouseEventMode = meSMInterpCtrlPoint;
		//}
		break;
	case meSMAddPoint:
	case meSMAddPointPrepare:
		if (isVertexSelectable(worldPos) && (m_interpolatePointsController.polyLine().size() == 0)) {
			m_mouseEventMode = meSMAddPointPrepare;
		} else {
			m_mouseEventMode = meSMAddPoint;
		}
		break;
	case meBreakLineAdd:
	case meBreakLineAddNotPossible:
		m_mouseEventMode = meBreakLineAddNotPossible;
		m_selectedVertexId = m_vtkPointLocator->FindClosestPoint(tmppos);
		m_mouseEventMode = meBreakLineAdd;
		pushRenderCommand(new BreakLineAddPointCommand(true, m_selectedVertexId, this));
		break;
	case meBreakLineRemove:
	case meBreakLineRemoveNotPossible:
		m_mouseEventMode = meBreakLineRemoveNotPossible;
		if (m_activeBreakLine != nullptr) {m_activeBreakLine->setActive(false);}
		m_activeBreakLine = nullptr;
		for (int i = 0; i < m_breakLines.count(); ++i) {
			GeoDataPointmapBreakLine* breakLine = m_breakLines[i];
			if (breakLine->isEdgeSelectable(worldPos, graphicsView()->stdRadius(iRIC::nearRadius()))) {
				m_mouseEventMode = meBreakLineRemove;
				m_activeBreakLine = breakLine;
				break;
			}
		}
		break;
	default:
		break;
	}
}

bool GeoDataPointmap::isVertexSelectable(const QPointF& pos)
{
	double point2[3] = {pos.x(), pos.y(), 0.0};
	m_selectedVertexId = m_vtkPointLocator->FindClosestPoint(point2);
	double point[3];
	m_vtkGrid->GetPoint(m_selectedVertexId, point);
	m_selectedZPos = m_vtkGrid->GetPointData()->GetArray(VALUES)->GetTuple1(m_selectedVertexId);
	QPointF vertexPos(point[0], point[1]);
	double limitdist = graphicsView()->stdRadius(iRIC::nearRadius());
	double dist = iRIC::distance(pos, vertexPos);
	return (dist < limitdist);
}

void GeoDataPointmap::updateMouseCursor(PreProcessorGraphicsViewInterface* v)
{
	switch (m_mouseEventMode) {
	case meNormal:
		v->setCursor(Qt::ArrowCursor);
		break;
	case meNormalWithShift:
		v->setCursor(m_addCursor);
		break;
	case meSMBox:
		v->setCursor(Qt::ArrowCursor);
		break;
	case meSMPolygon:
		v->setCursor(Qt::ArrowCursor);
		break;
	case meSMInterpPointNotPossible:
		v->setCursor(Qt::ArrowCursor);
		break;
	case meSMInterpPointPrepare:
		v->setCursor(m_interpPointAddCursor);
		break;
	case meSMInterpCtrlPoint:
		v->setCursor(Qt::CrossCursor);
		break;
	case meSMInterpPoint:
		v->setCursor(Qt::ArrowCursor);
		break;
	case meSMAddPoint:
		v->setCursor(Qt::ArrowCursor);
		break;
	case meSMAddPointPrepare:
		v->setCursor(m_interpPointAddCursor);
		break;
	case meSMAddCtrlPoint:
		v->setCursor(Qt::CrossCursor);
		break;
	case meBreakLineAddNotPossible:
		v->setCursor(Qt::ArrowCursor);
		break;
	case meBreakLineAdd:
		v->setCursor(m_addCursor);
		break;
	case meBreakLineRemoveNotPossible:
		v->setCursor(Qt::ArrowCursor);
		break;
	case meBreakLineRemove:
		v->setCursor(m_removeCursor);
		break;
	default:
		break;
	}
}

void GeoDataPointmap::remeshTINS(bool nodialog)
{
	QTime time;
	time.start();
	bool ok = doDelaunay(! nodialog);
	qDebug("Whole time for doDelaunay():%d", time.elapsed());
	if (! ok) {return;}
	graphicsView()->setCursor(Qt::WaitCursor);
	time.restart();
	renderGraphicsView();
	qDebug("Time elapsed for renderGraphicsView():%d", time.elapsed());
	// This operation is not undo-able.
	iRICUndoStack::instance().clear();
	graphicsView()->setCursor(Qt::ArrowCursor);
	if (! nodialog) {
		QMessageBox::information(preProcessorWindow(), tr("Information"), tr("Remeshing TINs finished."));
	}
}

void GeoDataPointmap::addBreakLine()
{
	if (m_mouseEventMode == meBreakLineAdd || m_mouseEventMode == meBreakLineAddNotPossible) {
		// cancel defining break line.
		pushRenderCommand(new BreakLineCancelDefinitionCommand(this));
	} else {
		if (m_representation == GeoDataPointmapRepresentationDialog::Surface) {
			int result = QMessageBox::information(preProcessorWindow(), tr("Information"), tr("When you add break line, you have to switch to show wireframes. Do you want to switch to show wireframes now?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
			if (result == QMessageBox::Yes) {
				m_representation = GeoDataPointmapRepresentationDialog::Wireframe;
				updateRepresentation();
				renderGraphicsView();
			} else {
				return;
			}
		}
		clearPointsSelection();
		// add new break line.
		InformationDialog::information(preProcessorWindow(), tr("Information"), tr("Break line can be defined by mouse-clicking on the points between which you want to add break line. Finish defining by double clicking, or pressing return key."), "geodatapointmapaddbreakline");
		pushRenderCommand(new BreakLineAddCommand(this));
	}
}

void GeoDataPointmap::removeBreakLine()
{
	if (m_mouseEventMode == meBreakLineRemoveNotPossible || m_mouseEventMode == meBreakLineRemove) {
		m_mouseEventMode = meNormal;
	} else {
		if (m_representation == GeoDataPointmapRepresentationDialog::Surface) {
			int result = QMessageBox::information(preProcessorWindow(), tr("Information"), tr("When you remove break line, you have to switch to show wireframes. Do you want to switch to show wireframes now?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
			if (result == QMessageBox::Yes) {
				m_representation = GeoDataPointmapRepresentationDialog::Wireframe;
				updateRepresentation();
				renderGraphicsView();
			} else {
				return;
			}
		}
		clearPointsSelection();
		InformationDialog::information(preProcessorWindow(), tr("Information"), tr("Click on the break line you want to remove."), "geodatapointmapremovebreakline");
		m_mouseEventMode = meBreakLineRemoveNotPossible;
	}
	updateActionStatus();
	updateMouseCursor(graphicsView());
	m_needRemeshing = true;
}

void GeoDataPointmap::removeAllBreakLines()
{
	int result = QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Are you sure you want to remove ALL break lines?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if (result == QMessageBox::No) {return;}
	clearPointsSelection();
	for (int i = 0; i < m_breakLines.count(); ++i) {
		delete m_breakLines[i];
	}
	m_breakLines.clear();
	// This operation is not undo-able.
	iRICUndoStack::instance().clear();
	renderGraphicsView();
	m_needRemeshing = true;
}

void GeoDataPointmap::removeTrianglesWithLongEdgeStart()
{
	if (m_representation == GeoDataPointmapRepresentationDialog::Points) {
		int result = QMessageBox::information(preProcessorWindow(), tr("Information"), tr("When you want to remove triangles with long edges, you have to switch Display Method to Wireframe or Surface. If you press OK button, Display Method is switched to Wireframe"), QMessageBox::Ok | QMessageBox::Cancel);
		if (result == QMessageBox::Cancel) {return;}

		if (m_needRemeshing) {
			remeshTINS();
		}
		m_representation = GeoDataPointmapRepresentationDialog::Wireframe;
		updateActorSettings();
	}
	m_longEdgeRemover = new TrianglesWithLongEdgeRemover(this);
}

void GeoDataPointmap::removeTrianglesWithLongEdgeEnd()
{
	delete m_longEdgeRemover;
	m_longEdgeRemover = nullptr;
}

void GeoDataPointmap::editPoints()
{
	if (m_representation != GeoDataPointmapRepresentationDialog::Points) {
		// @todo message review is needed.
		int result = QMessageBox::information(preProcessorWindow(), tr("Information"), tr("When you edit points, you have to switch to show points. Do you want to switch to show points now?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
		if (result == QMessageBox::Yes) {
			m_representation = GeoDataPointmapRepresentationDialog::Points;
			updateRepresentation();
			renderGraphicsView();
		} else {
			return;
		}
	}
	GeoDataPointmapEditPtsDialog dialog(this, preProcessorWindow());
	if (dialog.exec() != QDialog::Accepted) {return;}
	QVector<vtkIdType> selectedV = selectedVertices();
	if (selectedV.count() == 1) {
		pushRenderCommand(new EditSinglePointCommand(dialog.pointX(), dialog.pointY(), dialog.value(), selectedV[0], this));
	} else {
		pushRenderCommand(new EditPointsCommand(dialog.value(), selectedV, this));
	}
}

void GeoDataPointmap::editPointsDelete()
{
	if (m_representation != GeoDataPointmapRepresentationDialog::Points) {
		// @todo message review is needed.
		int result = QMessageBox::information(preProcessorWindow(), tr("Information"), tr("When you delete points, you have to switch to show points. Do you want to switch to show points now?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
		if (result == QMessageBox::Yes) {
			m_representation = GeoDataPointmapRepresentationDialog::Points;
			updateRepresentation();
			renderGraphicsView();
		} else {
			return;
		}
	}
	if (pointsUsedForBreakLines(selectedVertices())) {
		QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("You can not delete points used for break lines."));
		return;
	}
	pushRenderCommand(new DeletePointsCommand(tr("Delete Points"), selectedVertices(), this));
	clearPointsSelection();
}

void GeoDataPointmap::editPointsExport()
{
	QString defName =  QDir(LastIODirectory::get()).absoluteFilePath("file.tpo");
	QString fname = QFileDialog::getSaveFileName(
										projectData()->mainWindow(), tr("Select File to Export"), defName, tr("Topography File (*.tpo)"));
	if (fname.isNull()) {
		return;
	}
	QFile file(fname);
	if (! file.open(QIODevice::WriteOnly)) {
		QMessageBox::critical(preProcessorWindow(), tr("Error"), tr("Error occured while exporting to %1.").arg(fname));
		return;
	}
	QTextStream outs(&file);
	outs.setRealNumberNotation(QTextStream::SmartNotation);
	outs.setRealNumberPrecision(10);

	outs << m_selectedVerticesGrid->GetNumberOfCells() << endl;
	vtkPoints* points = m_vtkGrid->GetPoints();
	vtkDoubleArray* values = vtkDoubleArray::SafeDownCast(m_vtkGrid->GetPointData()->GetArray(VALUES));
	double v[3], val;
	vtkIdType* pts = 0, npts2;
	m_selectedVerticesGrid->GetVerts()->InitTraversal();
	for (vtkIdType j = 0; j < m_selectedVerticesGrid->GetVerts()->GetNumberOfCells(); ++j) {
		///*vtkCell* cell = */m_selectedVerticesGrid->GetVerts()->GetCell(j, npts, pts);
		//vtkIdType pointId = cell->GetPointId(0);
		m_selectedVerticesGrid->GetVerts()->GetNextCell(npts2, pts);
		vtkIdType pointId = pts[0];
		points->GetPoint(pointId, v);
		val = values->GetValue(pointId);
		outs << v[0] << " " << v[1] << " " << val << endl;
	}
	file.close();
}

void GeoDataPointmap::editPointsLessThan()
{
	if (m_representation != GeoDataPointmapRepresentationDialog::Points) {
		// @todo message review is needed.
		int result = QMessageBox::information(preProcessorWindow(), tr("Information"), tr("When you delete points, you have to switch to show points. Do you want to switch to show points now?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
		if (result == QMessageBox::Yes) {
			m_representation = GeoDataPointmapRepresentationDialog::Points;
			updateRepresentation();
			renderGraphicsView();
		} else {
			return;
		}
	}
	GeoDataPointMapDelPtsLessThanDialog dialog(this, preProcessorWindow());
	if (dialog.exec() != QDialog::Accepted) {return;}
	QString title(tr("Delete points less than %1"));
	QVector<vtkIdType> deletedPoints = dialog.selectedPoints();
	if (pointsUsedForBreakLines(deletedPoints)) {
		QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("You can not delete points used for break lines."));
		return;
	}
	pushRenderCommand(new DeletePointsCommand(title.arg(dialog.limitValue()), deletedPoints, this));
	clearPointsSelection();
}

void GeoDataPointmap::editPointsGreaterThan()
{
	if (m_representation != GeoDataPointmapRepresentationDialog::Points) {
		// @todo message review is needed.
		int result = QMessageBox::information(preProcessorWindow(), tr("Information"), tr("When you delete points, you have to switch to show points. Do you want to switch to show points now?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
		if (result == QMessageBox::Yes) {
			m_representation = GeoDataPointmapRepresentationDialog::Points;
			updateRepresentation();
			renderGraphicsView();
		} else {
			return;
		}
	}
	GeoDataPointmapDelPtsGreaterThanDialog dialog(this, preProcessorWindow());
	if (dialog.exec() != QDialog::Accepted) {return;}
	QString title(tr("Delete points greater than %1"));
	QVector<vtkIdType> deletedPoints = dialog.selectedPoints();
	if (pointsUsedForBreakLines(deletedPoints)) {
		QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("You can not delete points used for break lines."));
		return;
	}
	pushRenderCommand(new DeletePointsCommand(title.arg(dialog.limitValue()), deletedPoints, this));
	clearPointsSelection();
}

void GeoDataPointmap::mergePointmaps()
{
	auto gItem = dynamic_cast<PreProcessorGeoDataGroupDataItemInterface*> (parent()->parent());
	std::vector<PreProcessorGeoDataDataItemInterface*> dataToMerge;

	for (auto item : gItem->childItems()) {
		auto p = dynamic_cast<PreProcessorGeoDataDataItemInterface*> (item);
		auto geoData = p->geoData();
		if (geoData == this) {continue;}

		if (dynamic_cast<GeoDataPointmap*> (geoData) != nullptr) {
			dataToMerge.push_back(p);
		}
	}
	if (dataToMerge.size() == 0) {
		QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("There is no other data to merge in \"%1\" group.").arg(gItem->condition()->caption()));
		return;
	}

	GeoDataPointmapMergeSettingDialog dialog(preProcessorWindow());
	dialog.setItems(dataToMerge);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	dataToMerge = dialog.selectedItems();

	std::set<QPointF, QPointFCompare> pointSet;

	// insert points in this data to pointSet first
	auto myPoints = m_vtkGrid->GetPoints();
	auto myValues = vtkDoubleArray::SafeDownCast(m_vtkGrid->GetPointData()->GetArray(VALUES));
	auto myVerts = m_vtkGrid->GetVerts();

	double v[3];
	for (vtkIdType i = 0; i < myPoints->GetNumberOfPoints(); ++i) {
		myPoints->GetPoint(i, v);
		pointSet.insert(QPointF(v[0], v[1]));
	}

	auto duplicatePoints = 0;
	for (auto item : dataToMerge) {
		auto geoData = item->geoData();
		auto pointMap = dynamic_cast<GeoDataPointmap*> (geoData);

		auto points = pointMap->m_vtkGrid->GetPoints();
		auto vals = vtkDoubleArray::SafeDownCast(pointMap->m_vtkGrid->GetPointData()->GetArray(VALUES));

		double val;
		vtkIdType vId;
		for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
			points->GetPoint(i, v);
			val = vals->GetValue(i);
			QPointF p(v[0], v[1]);
			auto it = pointSet.find(p);
			if (it != pointSet.end()) {
				++ duplicatePoints;
				continue;
			}
			vId = myPoints->GetNumberOfPoints();

			myPoints->InsertNextPoint(v);
			myValues->InsertNextValue(val);
			myVerts->InsertNextCell(1, &vId);
			pointSet.insert(p);
		}

		item->setDeleteSilently(true);
		delete item;
	}
	m_vtkGrid->Modified();
	myPoints->Modified();
	myValues->Modified();
	myVerts->Modified();

	doDelaunay();
	updateActorSettings();
	renderGraphicsView();

	if (duplicatePoints > 0) {
		QMessageBox::information(preProcessorWindow(), tr("Information"), tr("%1 duplicate points were found in merge targets, and omitted.").arg(duplicatePoints));
	}

	// merge operation is not undoable.
	iRICUndoStack::instance().clear();
}

void GeoDataPointmap::clearPointsSelection()
{
	m_selectedVerticesGrid->Reset();
	m_selectedVerticesGrid->Allocate(10, 10);
	m_selectedVerticesGrid->SetPoints(m_vtkGrid->GetPoints());
	m_selectedVerticesGrid->Modified();
	renderGraphicsView();
}

void GeoDataPointmap::interpolatePoints(bool on)
{
	clearPointsSelection();

	if (on) {
		if (m_representation != GeoDataPointmapRepresentationDialog::Points) {
			int result = QMessageBox::information(preProcessorWindow(), tr("Information"), tr("When you interpolate points, you have to switch to show points. Do you want to switch to show points now?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
			if (result == QMessageBox::No) {return;}

			m_representation = GeoDataPointmapRepresentationDialog::Points;
			updateRepresentation();
		}
		InformationDialog::information(preProcessorWindow(), tr("Information"), tr("To interpolate points, select existing points by mouse-clicking, use Alt to define path between known points, and finish by double-clicking or pressing return key."), "geodatapointmapinterpolatepoint");
		m_mouseEventMode = GeoDataPointmap::meSMInterpPoint;
	} else {
		m_mouseEventMode = meNormal;
	}
	m_interpolatePointsController.clear();
	m_interpolateValues.clear();
	m_interpolateNewFlags.clear();

	updateActionStatus();
	renderGraphicsView();
}

void GeoDataPointmap::addPoints(bool on)
{
	clearPointsSelection();
	if (on) {
		if (m_representation == GeoDataPointmapRepresentationDialog::Surface) {
			int result = QMessageBox::information(preProcessorWindow(), tr("Information"), tr("When you add points, you have to switch to show points. Do you want to switch to show points now?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
			if (result == QMessageBox::No) {return;}

			m_representation = GeoDataPointmapRepresentationDialog::Points;
			updateRepresentation();
		}
		m_mouseEventMode = GeoDataPointmap::meSMAddPoint;
		InformationDialog::information(preProcessorWindow(), tr("Information"), tr("To add new points, select an existing point first. The value at that point will be used as the "
																	 "default value for new points. Then, click at any position you want to add points, and finish by double-clicking of pressing return key."), "geodatapointmapaddpoint");
	} else {
		m_interpolatePointsController.clear();
		m_mouseEventMode = meNormal;
	}
	updateActionStatus();
	renderGraphicsView();
}

void GeoDataPointmap::enablePointSelectedActions(bool val)
{
	m_editPointsAction->setEnabled(val);
	m_editPointsDeleteAction->setEnabled(val);
	m_editPointsExportAction->setEnabled(val);
	m_editPointsGreaterThanAction->setEnabled(val);
	m_editPointsLessThanAction->setEnabled(val);
}

vtkPolyData* GeoDataPointmap::selectedVerticesGrid() const
{
	return m_selectedVerticesGrid;
}

QVector<vtkIdType> GeoDataPointmap::selectedVertices()
{
	QSet<vtkIdType> tmpsel;
	QVector<vtkIdType> selected;
//	qDebug("Selected Points in m_selectedVerticesGrid:");
	vtkIdType numPoints;
	vtkIdType* points;
	m_selectedVerticesGrid->GetVerts()->InitTraversal();
	for (vtkIdType cellId = 0; cellId < m_selectedVerticesGrid->GetVerts()->GetNumberOfCells(); ++cellId) {
		m_selectedVerticesGrid->GetVerts()->GetNextCell(numPoints, points);
		for (int ptid = 0; ptid < numPoints; ++ptid) {
			tmpsel.insert(*(points + ptid));
//			qDebug(" pointId %d", *(points + ptid));
		}
	}
//	qDebug("Selected Points in m_selectedVerticesGrid End");
	for (auto it = tmpsel.begin(); it != tmpsel.end(); ++it) {
		selected.push_back(*it);
	}
	qSort(selected);
	return selected;
}

void GeoDataPointmap::updateBreakLinesOnDelete(QVector<vtkIdType>& deletedPoints)
{
	QList<vtkIdType> keyIds;
	QList<int> offsets;

	// build lists first.
	vtkIdType prevDeletedId = -1;
	vtkIdType keyIdToAddCandidate = 0;
	vtkIdType offsetToAddCandidate = 0;

	for (int i = 0; i < deletedPoints.size(); ++i) {
		vtkIdType deletedId = deletedPoints[i];
		if (deletedId != prevDeletedId + 1) {
			// Continuous
			keyIds.append(keyIdToAddCandidate);
			offsets.append(offsetToAddCandidate);
		} else {
			// Not continuous, so not used.
		}
		keyIdToAddCandidate = deletedId + 1;
		++ offsetToAddCandidate;

		prevDeletedId = deletedId;
	}
	keyIds.append(keyIdToAddCandidate);
	offsets.append(offsetToAddCandidate);

	foreach(GeoDataPointmapBreakLine* line, m_breakLines) {
		QVector<vtkIdType> oldindices = line->vertexIndices();
		QVector<vtkIdType> newindices;
		for (int i = 0; i < oldindices.count(); ++i) {
			vtkIdType oldindex = oldindices[i];
			auto lb = qLowerBound(keyIds, oldindex);
			vtkIdType tmpId = *lb;
			int listIndex;
			if (lb == keyIds.end()) {
				-- lb;
				tmpId = *lb;
				listIndex = keyIds.indexOf(tmpId);
			} else {
				listIndex = keyIds.indexOf(tmpId);
				if (tmpId != oldindex) {
					-- listIndex;
				}
			}
			int offset = offsets.at(listIndex);
			newindices.append(oldindex - offset);
		}
		line->setVertexIndices(newindices);
	}
}

void GeoDataPointmap::updateBreakLinesOnInsert(QVector<vtkIdType>& deletedPoints)
{
	QList<vtkIdType> keyIds;
	QList<int> offsets;

	// build lists first.
	vtkIdType prevDeletedId = -1;
	vtkIdType keyIdToAddCandidate = 0;
	vtkIdType offsetToAddCandidate = 0;

	for (int i = 0; i < deletedPoints.size(); ++i) {
		vtkIdType deletedId = deletedPoints[i];
		if (deletedId != prevDeletedId + 1) {
			// Continuous
			keyIds.append(keyIdToAddCandidate);
			offsets.append(offsetToAddCandidate);
		} else {
			// Not continuous, so not used.
		}
		keyIdToAddCandidate = deletedId - offsetToAddCandidate;
		++ offsetToAddCandidate;

		prevDeletedId = deletedId;
	}
	keyIds.append(keyIdToAddCandidate);
	offsets.append(offsetToAddCandidate);

	foreach (GeoDataPointmapBreakLine* line, m_breakLines) {
		QVector<vtkIdType> oldindices = line->vertexIndices();
		QVector<vtkIdType> newindices;
		for (int i = 0; i < oldindices.count(); ++i) {
			vtkIdType oldindex = oldindices[i];
			auto lb = qLowerBound(keyIds, oldindex);
			vtkIdType tmpId = *lb;
			int listIndex;
			if (lb == keyIds.end()) {
				-- lb;
				tmpId = *lb;
				listIndex = keyIds.indexOf(tmpId);
			} else {
				listIndex = keyIds.indexOf(tmpId);
				if (tmpId != oldindex) {
					-- listIndex;
				}
			}
			int offset = offsets.at(listIndex);
			newindices.append(oldindex + offset);
		}
		line->setVertexIndices(newindices);
	}
}

bool GeoDataPointmap::pointsUsedForBreakLines(const QVector<vtkIdType>& points)
{
	for (int i = 0; i < m_breakLines.count(); ++i) {
		GeoDataPointmapBreakLine* line1 = m_breakLines[i];
		const QVector<vtkIdType>& linePoints = line1->vertexIndices();

		for (int j = 0; j < linePoints.count(); ++j) {
			vtkIdType linePoint = linePoints[j];
			auto it = qFind(points, linePoint);
			if (it != points.end()) {
				// found.
				return true;
			}
		}
	}
	return false;
}

iRICLib::H5CgnsGeographicDataGroup::Type GeoDataPointmap::iRICLibType() const
{
	return iRICLib::H5CgnsGeographicDataGroup::Type::Pointmap;
}

void GeoDataPointmap::TSplineSTL(std::vector<double>& x, std::vector<double>& y, int n,
								std::vector<double>& xout, std::vector<double>& yout, int iout, float sigma,
								std::vector<double>& yp, std::vector<double>& temp)
{

	// subroutine variables
	double delx1, dx1, delx2, delx12, c1, c2, c3, slpp1, deln, delnm1;
	double delnn, slppn, sigmap, dels, exps, sinhs, sinhin;
	double diag1, diag2, diagin, spdiag, dx2;
	double a, b, del1, del2, exps1, sinhd1, sinhd2;
	double tmp;
	int nm1, np1, i, ibak, nj;

	Q_UNUSED(tmp)

	nm1 = n-2; //may need to make adjustments here because of c arrays 0-n vs 1-n for fortran
	np1 = n;
	delx1 = x[1]-x[0];
	dx1 = (y[1]-y[0])/delx1;
	delx2 = x[2]-x[1];
	delx12 = x[2]-x[0];
	c1 = -(delx12+delx1)/delx12/delx1;
	c2 = delx12/delx1/delx2;
	c3 = -delx1/delx12/delx2;
	slpp1 = c1*y[0]+c2*y[1]+c3*y[2];
	deln = x[n-1]-x[nm1];
	delnm1 = x[nm1]-x[n-3];
	delnn = x[n-1]-x[n-3];
	c1 = (delnn+deln)/delnn/deln;
	c2 = -delnn/deln/delnm1;
	c3 = deln/delnn/delnm1;
	slppn = c3*y[n-3]+c2*y[nm1]+c1*y[n-1];
	sigmap = fabs(sigma)*(double(n-2))/(x[n-1]-x[0]);
	dels = sigmap*delx1;
	exps = exp(dels);
	sinhs = 0.5*(exps-1./exps);
	sinhin = 1./(delx1*sinhs);
	diag1 = sinhin*(dels*0.5*(exps+1./exps)-sinhs);
	diagin = 1./diag1;
	yp[0] = diagin*(dx1-slpp1);
	spdiag = sinhin*(sinhs-dels);
	temp[0] = diagin* spdiag;
	for (i = 1; i <= nm1; i++) {
		delx2 = x[i+1] - x[i];
		dx2 = (y[i+1]-y[i])/delx2;
		dels = sigmap*delx2;
		exps = exp(dels);
		sinhs = 0.5*(exps-1./exps);
		sinhin = 1./(delx2*sinhs);
		diag2 = sinhin*(dels*(0.5*(exps+1./exps))-sinhs);
		diagin = 1./(diag1+diag2-spdiag*temp[i-1]);
		yp[i] = diagin*(dx2-dx1-spdiag*yp[i-1]);
		spdiag = sinhin*(sinhs-dels);
		temp[i] = diagin*spdiag;
		dx1 = dx2;
		diag1 = diag2;
	}
	diagin = 1./(diag1-spdiag*temp[nm1]);
	yp[n-1] = diagin*(slppn-dx2-spdiag*yp[nm1]);
	for (i = 1; i <= n - 1; i++) {
		ibak = np1-i-1;
		yp[ibak] =yp[ibak]-temp[ibak]*yp[ibak+1];
	}
	a = x[0];
	b = x[1];
	nj = 1;
	for (i = 0; i <= iout - 1; i++) {
		while (xout[i] > b) {
			a = b;
			nj = nj+1;
			if (nj < n) {
				b = x[nj];
			}
		}
		del1 = xout[i] - a;
		del2 = b-xout[i];
		dels = b-a;
		exps1 = exp(sigmap*del1);
		sinhd1 = 0.5*(exps1-1./exps1);
		exps = exp(sigmap*del2);
		sinhd2 = 0.5*(exps-1./exps);
		exps = exps*exps1;
		sinhs = 0.5*(exps-1./exps);
		tmp = (yp[nj]*sinhd1+yp[nj-1]*sinhd2)/
					sinhs+((y[nj]-yp[nj])*del1+(y[nj-1]-yp[nj-1])*del2)/dels;
		yout[i] = (yp[nj]*sinhd1+yp[nj-1]*sinhd2)/
							sinhs+((y[nj]-yp[nj])*del1+(y[nj-1]-yp[nj-1])*del2)/dels;
	}
}

void GeoDataPointmap::finishAddPoint()
{
	if (m_interpolatePointsController.polyLine().size() < 2) {
		int ret = QMessageBox::information(preProcessorWindow(), tr("Warning"), tr("You must select one known point, and define at least one location for added points. "
																																		 "Do you want to retry, or cancel the operation?"), QMessageBox::Retry | QMessageBox::Cancel, QMessageBox::Cancel);
		if (ret == QMessageBox::Retry) {
			return;
		};
	} else {
		bool ok;
		double val = QInputDialog::getDouble(preProcessorWindow(), tr("Add Points"), tr("Values of new points"), m_addPointsValue, -10000, 10000, 3, &ok);
		if (ok) {
			auto newPoints = m_interpolatePointsController.polyLine();
			newPoints.erase(newPoints.begin());
			std::vector<double> vals;
			for (int i = 0; i < newPoints.size(); ++i) {
				vals.push_back(val);
			}
			iRICUndoStack::instance().clear(); // to avoid crash while undoing
			pushRenderCommand(new AddPointsCommand(newPoints, vals, this));
		}
	}
	m_interpolatePointsController.clear();
	m_interpolateValues.clear();
	m_interpolateNewFlags.clear();
	m_mouseEventMode = meNormal;
	updateActionStatus();
}

void GeoDataPointmap::finishInterpPoint()
{
	if (! (m_interpolateNewFlags.at(m_interpolateNewFlags.size() - 1) == false && m_interpolatePointsController.polyLine().size() > 1)) {
		int ret = QMessageBox::information(preProcessorWindow(), tr("Warning"), tr("To interpolate points, you must specify more than two points, and the last point should be existing point. "
																																							 "Do you want to retry, or cancel the operation?"), QMessageBox::Retry | QMessageBox::Cancel, QMessageBox::Cancel);
		if (ret == QMessageBox::Retry) {
			return;
		};
	} else {
		double len = 0;
		std::vector<double> xarr, yarr, zarr;
		std::vector<double> lens_xy;
		std::vector<double> lens_z;

		auto line = m_interpolatePointsController.polyLine();

		lens_xy.push_back(0);
		lens_z.push_back(0);

		for (int i = 0; i < line.size(); ++i) {
			auto p = line.at(i);
			xarr.push_back(p.x());
			yarr.push_back(p.y());
			if (m_interpolateNewFlags.at(i) == false) {
				zarr.push_back(m_interpolateValues.at(i));
			}
		}

		for (int i = 0; i < line.size() - 1; ++i) {
			auto p1 = line.at(i);
			auto p2 = line.at(i + 1);
			len += iRIC::distance(p1, p2);
			lens_xy.push_back(len);

			if (m_interpolateNewFlags[i + 1] == false) {
				lens_z.push_back(len);
			}
		}
		GeoDataPointmapInterpolatePointsDialog dialog(preProcessorWindow());
		dialog.setLineLength(len);
		int ret = dialog.exec();
		if (ret == QDialog::Accepted) {
			double increment = dialog.increment();

			std::vector<QPointF> points;
			std::vector<double> xpoint, ypoint, values;

			// number of points to be added by interpolation
			int numNewPoints = static_cast<int>(len / increment) + 1;

			xpoint.assign(numNewPoints, 0);
			ypoint.assign(numNewPoints, 0);
			values.assign(numNewPoints, 0);

			std::vector<double> sout, yp, tmp;
			sout.assign(numNewPoints, 0);
			yp.assign(numNewPoints, 0);
			tmp.assign(numNewPoints, 0);
			for (int i = 0; i < numNewPoints; ++i) {
				sout[i] = i * len / (numNewPoints - 1);
			}

			// interpolation for x and y
			if (lens_xy.size() == 2) {
				for (int i = 0; i < numNewPoints; ++i) {
					double t = i / static_cast<double>(numNewPoints - 1);
					xpoint[i] = xarr.at(0) * (1 - t) + xarr.at(1) * t;
					ypoint[i] = yarr.at(0) * (1 - t) + yarr.at(1) * t;
				}
			} else {
				// spline interpolation for x and y
				TSplineSTL(lens_xy, xarr, xarr.size(), sout, xpoint, numNewPoints, 5.0, yp, tmp);
				TSplineSTL(lens_xy, yarr, yarr.size(), sout, ypoint, numNewPoints, 5.0, yp, tmp);
			}

			// interpolation for z
			if (lens_z.size() == 2) {
				for (int i = 0; i < numNewPoints; ++i) {
					double t = i / static_cast<double>(numNewPoints - 1);
					values[i] = zarr.at(0) * (1 - t) + zarr.at(1) * t;
				}
			} else {
				// spline interpolation for z
				TSplineSTL(lens_z, zarr, zarr.size(), sout, values, numNewPoints, 10.0, yp, tmp);
			}

			// remove first and last points, because they are points already exists.
			xpoint.erase(xpoint.begin());
			xpoint.pop_back();
			ypoint.erase(ypoint.begin());
			ypoint.pop_back();
			values.erase(values.begin());
			values.pop_back();
			for (int i = 0; i < xpoint.size(); ++i) {
				points.push_back(QPointF(xpoint.at(i), ypoint.at(i)));
			}
			iRICUndoStack::instance().clear(); // to avoid crash while undoing
			pushRenderCommand(new AddPointsCommand(points, values, this));
		}
		m_interpolatePointsController.clear();
		m_mouseEventMode = meNormal;
		updateActionStatus();
	}
}

vtkCell* GeoDataPointmap::findCell(double x, double y, double *weights)
{
	auto env = new geos::geom::Envelope(x, x, y, y);
	std::vector<void*> ret;
	m_qTree->query(env, ret);
	delete env;

	double point[3], closestPoint[3], pcoords[3], dist;
	double bounds[6];
	int subId;

	point[0] = x;
	point[1] = y;
	point[2] = 0;

	for (void* vptr : ret) {
		vtkIdType cellId = reinterpret_cast<vtkIdType> (vptr);
		m_vtkDelaunayedPolyData->GetCellBounds(cellId, bounds);
		if (point[0] < bounds[0]) {continue;}
		if (point[0] > bounds[1]) {continue;}
		if (point[1] < bounds[2]) {continue;}
		if (point[1] > bounds[3]) {continue;}
		vtkCell* cell = m_vtkDelaunayedPolyData->GetCell(cellId);
		if (1 == cell->EvaluatePosition(point, closestPoint, subId, pcoords, dist, weights)) {
			return cell;
		}
	}
	return nullptr;
}

void GeoDataPointmap::doApplyOffset(double x, double y)
{
	vtkPoints* points = m_vtkDelaunayedPolyData->GetPoints();
	vtkPoints* gridPoints = m_vtkGrid->GetPoints();
	if (points == gridPoints) {
		// opened file was (projectData()->version().build() >= 3607)
		// see GeoDataPointmap::loadExternalData
		vtkIdType numPoints = points->GetNumberOfPoints();
		double v[3];
		for (vtkIdType id = 0; id < numPoints; ++id) {
			points->GetPoint(id, v);
			v[0] -= x;
			v[1] -= y;
			points->SetPoint(id, v);
		}
		points->Modified();
	} else {
		// opened file was (projectData()->version().build() < 3607)
		// see GeoDataPointmap::loadExternalData
		vtkIdType numPoints = points->GetNumberOfPoints();
		Q_ASSERT(numPoints == gridPoints->GetNumberOfPoints());
		double v[3];
		for (vtkIdType id = 0; id < numPoints; ++id) {
			points->GetPoint(id, v);
			v[0] -= x;
			v[1] -= y;
			points->SetPoint(id, v);
			gridPoints->SetPoint(id, v);
		}
		points->Modified();
		gridPoints->Modified();
	}
}

void GeoDataPointmap::rebuildQTree()
{
	delete m_qTree;
	m_qTree = new geos::index::quadtree::Quadtree();

	double bounds[6];
	for (vtkIdType i = 0; i < m_vtkDelaunayedPolyData->GetNumberOfCells(); ++i) {
		m_vtkDelaunayedPolyData->GetCellBounds(i, bounds);

		auto env = new geos::geom::Envelope(bounds[0], bounds[1], bounds[2], bounds[3]);
		m_qTree->insert(env, reinterpret_cast<void*>(i));
	}
}
