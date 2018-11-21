#define REAL double
#define VOID void

#include "geodatapointmap.h"
#include "geodatapointmapaddpointdialog.h"
#include "geodatapointmapbreakline.h"
#include "geodatapointmapdelptsgreaterthandialog.h"
#include "geodatapointmapdelptslessthandialog.h"
#include "geodatapointmapeditptsdialog.h"
#include "geodatapointmapinterpolatepoints.h"
#include "geodatapointmaprepresentationdialog.h"
#include "private/geodatapointmap_addinterpolatepointscommand.h"
#include "private/geodatapointmap_addpointcommand.h"
#include "private/geodatapointmap_addpointscommand.h"
#include "private/geodatapointmap_addpointsetreferencecommand.h"
#include "private/geodatapointmap_breaklineaddcommand.h"
#include "private/geodatapointmap_breaklineaddpointcommand.h"
#include "private/geodatapointmap_breaklinecanceldefinitioncommand.h"
#include "private/geodatapointmap_breaklinefinishdefinitioncommand.h"
#include "private/geodatapointmap_deletepointscommand.h"
#include "private/geodatapointmap_editpointscommand.h"
#include "private/geodatapointmap_editsinglepointcommand.h"
#include "private/geodatapointmap_interpolatelineaddpointcommand.h"
#include "private/geodatapointmap_triangleswithlongedgeremover.h"

#include <guibase/widget/waitdialog.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/misc/mouseboundingbox.h>
#include <guicore/misc/qundocommandhelper.h>
#include <guicore/pre/base/preprocessordataitem.h>
#include <guicore/pre/base/preprocessordatamodelinterface.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/base/preprocessorgeodatadataiteminterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <guicore/project/projectdata.h>
#include <guicore/scalarstocolors/scalarstocolorscontainer.h>
#include <misc/informationdialog.h>
#include <misc/iricundostack.h>
#include <misc/keyboardsupport.h>
#include <misc/lastiodirectory.h>
#include <misc/stringtool.h>
#include <misc/mathsupport.h>
#include <misc/versionnumber.h>
#include <misc/zdepthrange.h>
#include <triangle/triangle.h>
#include <triangle/triangleexecutethread.h>

#include <QAction>
#include <QApplication>
#include <QDomNode>
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

#include <iriclib_pointmap.h>

const char* GeoDataPointmap::VALUES = "values";

GeoDataPointmap::GeoDataPointmap(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* att) :
	GeoData {d, creator, att},
	m_longEdgeRemover {nullptr}
{
	doubleclick = false;
	m_vtkPolygon = vtkSmartPointer<vtkPolygon>::New();
	this->m_vtkInterpPolygon = vtkSmartPointer<vtkPolygon>::New();
	this->m_vtkInterpValue = vtkSmartPointer<vtkDoubleArray>::New();
	this->m_polyVertexGrid = vtkSmartPointer<vtkUnstructuredGrid>::New();
	this->m_polyEdgeGrid = vtkSmartPointer<vtkUnstructuredGrid>::New();

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

	this->m_InterpLineGrid = vtkSmartPointer<vtkUnstructuredGrid>::New();
	vtkPoints* points3 = vtkPoints::New();
	points3->SetDataTypeToDouble();
	m_InterpLineGrid->SetPoints(points3);
	points3->Delete();

	m_vtkDelaunayedPolyData = vtkSmartPointer<vtkPolyData>::New();
	m_vtkDelaunayedPolyData->Initialize();

	m_vtkPointLocator = vtkSmartPointer<vtkPointLocator>::New();
	m_vtkPointLocator->SetDataSet(m_vtkGrid);

	m_representation = GeoDataPointmapRepresentationDialog::Points;
	m_opacityPercent = 50;

	m_addPixmap = QPixmap(":/libs/guibase/images/cursorAdd.png");
	m_addCursor = QCursor(m_addPixmap, 0, 0);

	m_removePixmap = QPixmap(":/libs/guibase/images/cursorRemove.png");
	m_removeCursor = QCursor(m_removePixmap, 0, 0);

	m_interpPointAddPixmap = QPixmap(":/libs/guibase/images/cursorAdd.png");
	m_interpPointAddCursor = QCursor(m_interpPointAddPixmap, 0, 0);

	m_interpPointCtrlAddPixmap = QPixmap(":/images/cursorCtrlAdd.png");
	m_interpPointCtrlAddCursor = QCursor(m_interpPointCtrlAddPixmap, 0, 0);

	this->lastInterpPointKnown = false;

	setupActions();

	m_activeBreakLine = 0;
	this->selectionModePoint(true);

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
	r->RemoveActor(m_polyEdgeActor);
	r->RemoveActor(m_polyVertexActor);
	r->RemoveActor(m_selectedActor);
	r->RemoveActor(m_InterpLineActor);

	vtkActorCollection* col = actorCollection();
	col->RemoveAllItems();
}

vtkPolyData* GeoDataPointmap::vtkGrid() const
{
	return m_vtkGrid;
}

vtkPolyData* GeoDataPointmap::delaunayedPolyData() const
{
	return m_vtkDelaunayedPolyData;
}

void GeoDataPointmap::updateInterpShapeData()
{
	vtkPoints* interpPoints = m_vtkInterpPolygon->GetPoints();
	// update idlist.
	vtkIdList* idlist = m_vtkInterpPolygon->GetPointIds();
	idlist->Reset();
	idlist->SetNumberOfIds(interpPoints->GetNumberOfPoints());
	for (int i = 0; i < interpPoints->GetNumberOfPoints(); ++i) {
		idlist->SetId(i, i);
	}
	idlist->Modified();

	// Edge line representation.
	//m_InterpLineGrid->Reset();
	//int edgeCount = m_vtkInterpPolygon->GetNumberOfEdges();
	//m_InterpLineGrid->Allocate(edgeCount);
	//for (int i = 0; i < edgeCount; ++i){
	//	vtkCell* nextCell = m_vtkPolygon->GetEdge(i);
	//	m_InterpLineGrid->InsertNextCell(nextCell->GetCellType(), nextCell->GetPointIds());
	//}
	// Points representation.
	m_InterpLineGrid->Reset();
	vtkIdType vertexId = 0;
	int vertexCount = m_vtkInterpPolygon->GetNumberOfPoints();
	m_InterpLineGrid->Allocate(vertexCount);
	m_InterpLineGrid->SetPoints(m_vtkInterpPolygon->GetPoints());
	for (int i = 0; i < vertexCount; ++i) {
		vtkVertex* nextVertex = vtkVertex::New();
		nextVertex->GetPointIds()->SetId(0, vertexId);
		m_InterpLineGrid->InsertNextCell(nextVertex->GetCellType(), nextVertex->GetPointIds());
		vertexId += 1;
	}
	//m_polyVertexGrid->GetPointData()->AddArray(m_scalarValues);
	//m_polyVertexGrid->GetPointData()->SetActiveScalars("polygonvalue");
	m_InterpLineGrid->Modified();
}

void GeoDataPointmap::updateShapeData()
{
	vtkPoints* points = m_vtkPolygon->GetPoints();

	// update idlist.
	vtkIdList* idlist = m_vtkPolygon->GetPointIds();
	idlist->Reset();
	idlist->SetNumberOfIds(points->GetNumberOfPoints());
	for (int i = 0; i < points->GetNumberOfPoints(); ++i) {
		idlist->SetId(i, i);
	}
	idlist->Modified();

	// Edge line representation.
	m_polyEdgeGrid->Reset();
	int edgeCount = m_vtkPolygon->GetNumberOfEdges();
	m_polyEdgeGrid->Allocate(edgeCount);
	for (int i = 0; i < edgeCount; ++i) {
		vtkCell* nextCell = m_vtkPolygon->GetEdge(i);
		m_polyEdgeGrid->InsertNextCell(nextCell->GetCellType(), nextCell->GetPointIds());
	}
	m_polyEdgeGrid->SetPoints(m_vtkPolygon->GetPoints());
	m_polyEdgeGrid->BuildLinks();
	m_polyEdgeGrid->Modified();

	// Points representation.
	m_polyVertexGrid->Reset();
	vtkIdType vertexId = 0;
	int vertexCount = m_vtkPolygon->GetNumberOfPoints();
	m_polyVertexGrid->Allocate(vertexCount);
	for (int i = 0; i < vertexCount; ++i) {
		vtkVertex* nextVertex = vtkVertex::New();
		nextVertex->GetPointIds()->SetId(0, vertexId);
		m_polyVertexGrid->InsertNextCell(nextVertex->GetCellType(), nextVertex->GetPointIds());
		vertexId += 1;
	}
	m_polyVertexGrid->SetPoints(m_vtkPolygon->GetPoints());
	m_polyVertexGrid->Modified();

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
	in.pointattributelist = NULL;
	in.pointmarkerlist = NULL;
	in.numberofpoints = m_vtkGrid->GetPoints()->GetNumberOfPoints();
	in.numberofpointattributes = 0;

	in.trianglelist = NULL;
	in.triangleattributelist = NULL;
	in.trianglearealist = NULL;
	in.neighborlist = NULL;
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

	in.holelist = NULL;
	in.numberofholes = 0;

	in.regionlist = NULL;
	in.numberofregions = 0;

	in.edgelist = NULL;
	in.edgemarkerlist = NULL;
	in.normlist = NULL;
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
		m_InterpLineGrid->Reset();
		m_InterpLineGrid->Allocate(10, 10);
		m_InterpLineGrid->SetPoints(m_vtkGrid->GetPoints());
		m_InterpLineGrid->Modified();
		qDebug("m_InterpLineGrid Building:%d", time.elapsed());

		m_needRemeshing = false;
		qDebug("Time for constructiong VTK Objects:%d", time.elapsed());
	}

	trifree(out.pointmarkerlist);
	trifree(out.trianglelist);
	trifree(out.segmentlist);
	trifree(out.segmentmarkerlist);
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
//	m_pointsActor = vtkSmartPointer<vtkActor>::New();
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
	m_selectedMapper->SetInputData(this->m_selectedVerticesGrid);
	m_selectedMapper->ImmediateModeRenderingOn();

	m_InterpLineMapper = vtkSmartPointer<vtkDataSetMapper>::New();
	m_InterpLineMapper->SetScalarVisibility(false);
	m_InterpLineMapper->SetInputData(this->m_InterpLineGrid);
	m_InterpLineMapper->ImmediateModeRenderingOn();

	m_polyEdgeMapper = vtkSmartPointer<vtkDataSetMapper>::New();
	m_polyEdgeMapper->SetScalarVisibility(false);
	m_polyEdgeMapper->SetInputData(this->m_polyEdgeGrid);
	m_polyEdgeMapper->ImmediateModeRenderingOn();

	m_polyVertexMapper = vtkSmartPointer<vtkDataSetMapper>::New();
	m_polyVertexMapper->SetScalarVisibility(false);
	m_polyVertexMapper->SetInputData(this->m_polyEdgeGrid);
	m_polyVertexMapper->ImmediateModeRenderingOn();

	m_selectedActor = vtkSmartPointer<vtkActor>::New();
	m_selectedActor->GetProperty()->SetPointSize(9.);
	m_selectedActor->GetProperty()->SetLighting(false);
	m_selectedActor->GetProperty()->SetColor(0, 0, 0);
	m_selectedActor->SetMapper(m_selectedMapper);
	m_selectedActor->GetProperty()->SetRepresentationToPoints();
	m_selectedActor->SetVisibility(true);

	m_selectedActor->VisibilityOn();

	this->m_InterpLineActor = vtkSmartPointer<vtkActor>::New();
	this->m_InterpLineActor->GetProperty()->SetPointSize(7.);
	this->m_InterpLineActor->GetProperty()->SetLighting(true);
	this->m_InterpLineActor->GetProperty()->SetColor(0,0,0);
	this->m_InterpLineActor->SetMapper(this->m_InterpLineMapper);
	this->m_InterpLineActor->GetProperty()->SetRepresentationToPoints();
	this->m_InterpLineActor->SetVisibility(true);

	this->m_polyEdgeActor = vtkSmartPointer<vtkActor>::New();
	this->m_polyEdgeActor->GetProperty()->SetLineWidth(2);
	this->m_polyEdgeActor->GetProperty()->SetLighting(true);
	this->m_polyEdgeActor->GetProperty()->SetColor(0,0,0);
	this->m_polyEdgeActor->SetMapper(this->m_polyEdgeMapper);
	this->m_polyEdgeActor->SetVisibility(true);

	this->m_polyVertexActor = vtkSmartPointer<vtkActor>::New();
	this->m_polyVertexActor->GetProperty()->SetPointSize(5.0);
	this->m_polyVertexActor->GetProperty()->SetLighting(true);
	this->m_polyVertexActor->GetProperty()->SetColor(0,0,0);
	this->m_polyVertexActor->SetMapper(this->m_polyEdgeMapper);
	this->m_polyVertexActor->SetVisibility(true);

	vtkRenderer* r = renderer();
	r->AddActor(m_actor);
	r->AddActor(m_pointsActor);
	r->AddActor(m_polyVertexActor);
	r->AddActor(m_polyEdgeActor);
	r->AddActor(m_InterpLineActor);
	r->AddActor(m_selectedActor);

	actorCollection()->AddItem(m_polyVertexActor);
	actorCollection()->AddItem(m_polyEdgeActor);
	actorCollection()->AddItem(m_InterpLineActor);
	actorCollection()->AddItem(m_selectedActor);

	updateActorSettings();
}

void GeoDataPointmap::setupMenu()
{
	m_menu->setTitle(tr("P&ointset Data"));
	m_menu->addAction(m_editNameAction);
	m_menu->addSeparator();
	m_menu->addAction(this->m_selectionModePolygon);
	m_menu->addSeparator();
	m_menu->addAction(this->m_editPointsAction);
	m_menu->addAction(this->m_editPointsExportAction);
	m_menu->addSeparator();
	m_menu->addAction(this->m_addPointAction);
	m_menu->addAction(this->m_interpolatePointAction);
	m_menu->addSeparator();
	m_menu->addAction(this->m_editPointsDeleteAction);
	m_menu->addAction(this->m_editPointsLessThanAction);
	m_menu->addAction(this->m_editPointsGreaterThanAction);
	m_menu->addSeparator();
	m_menu->addAction(this->m_addBreakLineAction);
	m_menu->addAction(this->m_removeBreakLineAction);
	m_menu->addAction(this->m_removeAllBreakLinesAction);
	m_menu->addSeparator();
	m_menu->addAction(this->m_remeshAction);
	m_menu->addAction(this->m_removeTrianglesWithLongEdgeAction);
	m_menu->addSeparator();
	m_menu->addAction(m_displaySettingAction);
	m_menu->addSeparator();
	m_menu->addAction(deleteAction());

	m_rightClickingMenu = new QMenu();
	m_rightClickingMenu->addAction(this->m_selectionModePolygon);
	m_rightClickingMenu->addSeparator();
	m_rightClickingMenu->addAction(this->m_editPointsAction);
	m_rightClickingMenu->addAction(this->m_editPointsExportAction);
	m_rightClickingMenu->addSeparator();
	m_rightClickingMenu->addAction(this->m_addPointAction);
	m_rightClickingMenu->addAction(this->m_interpolatePointAction);
	m_rightClickingMenu->addSeparator();
	m_rightClickingMenu->addAction(this->m_editPointsDeleteAction);
	m_rightClickingMenu->addAction(this->m_editPointsLessThanAction);
	m_rightClickingMenu->addAction(this->m_editPointsGreaterThanAction);
	m_rightClickingMenu->addSeparator();
	m_rightClickingMenu->addAction(this->m_addBreakLineAction);
	m_rightClickingMenu->addAction(this->m_removeBreakLineAction);
	m_rightClickingMenu->addAction(this->m_removeAllBreakLinesAction);
	m_rightClickingMenu->addSeparator();
	m_rightClickingMenu->addAction(this->m_remeshAction);
	m_rightClickingMenu->addAction(this->m_removeTrianglesWithLongEdgeAction);
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
	tb->addAction(this->m_addPointAction);
	tb->addAction(this->m_interpolatePointAction);
	tb->addSeparator();
	tb->addAction(this->m_addBreakLineAction);
	tb->addAction(this->m_removeBreakLineAction);
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
	m_polyVertexActor->SetPosition(0, 0, range.max());
	m_polyEdgeActor->SetPosition(0, 0, range.max());
	m_InterpLineActor->SetPosition(0, 0, range.max());

	double breakLineDepth = range.max() * 0.1 + range.min() * 0.9;
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
}

void GeoDataPointmap::mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (event->button() == Qt::LeftButton) {
		// left click
		double worldX = static_cast<double>(event->x());
		double worldY = static_cast<double>(event->y());
		v->viewportToWorld(worldX, worldY);
		MouseBoundingBox* box = dataModel()->mouseBoundingBox();

		switch (m_mouseEventMode) {
		case meNormal:
		case meNormalWithShift:
			box->setStartPoint(event->x(), event->y());
			box->enable();
			v->GetRenderWindow()->SetDesiredUpdateRate(PreProcessorDataItem::dragUpdateRate);
			break;
		case meSMPolygonPrepare:
			//adds first point.  point added in meSMPolygon below is then modified on mouse move
			pushRenderCommand(new AddPointCommand(true, QPoint(event->x(), event->y()), this));
		case meSMPolygon:
			m_mouseEventMode = meSMPolygon;
			pushRenderCommand(new AddPointCommand(true, QPoint(event->x(), event->y()), this));
			break;
		case meSMInterpPointPrepare: {
				bool ptduplicate = false;
				vtkSmartPointer<vtkVertex> v = vtkSmartPointer<vtkVertex>::New();
				v->GetPointIds()->SetId(0, this->m_selectedVertexId2);
				double p[3];
				this->m_vtkGrid->GetPoint(m_selectedVertexId2, p);
				vtkSmartPointer<vtkIdList> idlist = vtkSmartPointer<vtkIdList>::New();
				int numcells = this->m_InterpLineGrid->GetNumberOfCells();
				for (vtkIdType i = 0; i < numcells; i++) {
					this->m_InterpLineGrid->GetCellPoints(i, idlist);
					if (m_selectedVertexId2 == static_cast<int>(idlist->GetId(0))) {
						ptduplicate = true;
						break;
					}
				}
				if (!ptduplicate) {
					pushRenderCommand(new InterpolateLineAddPointCommand(true, p[0], p[1], this->m_selectedZPos, this));
					lastInterpPointKnown = true;

					this->m_InterpLineGrid->Modified();
					this->lastInterpPointKnown = true;
				}
				//this->m_vtkSelectedPolyData = m_selectedVerticesGrid->GetData()
				this->m_mouseEventMode = meSMInterpPoint;
				this->m_selectedZPos = 0.;
			}
			break;
		case meSMInterpPoint:
			break;
		case meSMInterpCtrlPoint:
			pushRenderCommand(new InterpolateLineAddPointCommand(true, worldX, worldY, -9999., this));
			lastInterpPointKnown = false;
			this->m_InterpLineGrid->BuildLinks();
			this->m_InterpLineGrid->Modified();
			this->lastInterpPointKnown = false;
			break;
		case meSMAddPoint:
			break;
		case meSMAddCtrlPoint:
			if (this->m_vtkInterpPolygon->GetNumberOfPoints() > 0) {
				pushRenderCommand(new InterpolateLineAddPointCommand(true, worldX, worldY, -9999., this));
			}
			break;
		case meSMAddPointPrepare:
			if (this->m_vtkInterpPolygon->GetNumberOfPoints() == 0) {
				double p[3];
				this->m_vtkGrid->GetPoint(m_selectedVertexId2, p);
				pushRenderCommand(new InterpolateLineAddPointCommand(true, p[0], p[1], this->m_selectedZPos, this));
				this->m_mouseEventMode = meSMAddCtrlPoint;
			}
			break;
		case meBreakLineAddNotPossible:
			// do nothing.
			break;
		case meBreakLineAdd:
			// add point.
			pushRenderCommand(new BreakLineAddPointCommand(false, m_selectedVertexId2, this));
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
		pushRenderCommand(new AddPointCommand(false, QPoint(event->x(), event->y()), this));
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
		this->finishAddPoint();
	} else if (this->m_mouseEventMode == this->meSMInterpPointPrepare || m_mouseEventMode == meSMInterpPoint) {
		this->finishInterpPoint();
	} else if (m_mouseEventMode == meBreakLineAdd || m_mouseEventMode == meBreakLineAddNotPossible) {
		pushRenderCommand(new BreakLineFinishDefinitionCommand(this));
	}
	this->updateMouseCursor(v);
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
	case meSMAddCtrlPoint:
		//this->finishAddPoint();
		break;
	default:
		break;
	}

	if (iRIC::isEnterKey(event->key())) {
		if (m_mouseEventMode == meSMPolygon) {
			definePolygon(false, xOr);
		} else if (m_mouseEventMode == meSMAddCtrlPoint) {
			this->finishAddPoint();
		} else if (this->m_mouseEventMode == this->meSMInterpPointPrepare || m_mouseEventMode == meSMInterpPoint) {
			this->finishInterpPoint();
		} else if (m_mouseEventMode == meBreakLineAdd || m_mouseEventMode == meBreakLineAddNotPossible) {
			pushRenderCommand(new BreakLineFinishDefinitionCommand(this));
		}
	} else if (event->key() == Qt::Key_Alt) {
		if (m_mouseEventMode == meSMInterpPointPrepare) {
			m_mouseEventMode = meSMInterpCtrlPoint;
		} else if (m_mouseEventMode == meSMInterpPoint) {
			m_mouseEventMode = meSMInterpCtrlPoint;
		}
		//if((m_mouseEventMode == meSMAddPoint) && (this->m_vtkInterpPolygon->GetNumberOfPoints() > 0)) {
		//	m_mouseEventMode = meSMAddCtrlPoint;
		//}
	}
	this->updateMouseCursor(v);
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
	if (polygon().count() <= minCount) {
		QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Polygon must have three vertices at least."));
		return;
	}
	iRICUndoStack& stack = iRICUndoStack::instance();
	stack.undo();
	this->showPolygonSelectedPoints(xOr);
	this->resetSelectionPolygon();
	this->m_mouseEventMode = GeoDataPointmap::meNormal;
	//this->updateMouseCursor(m_pointMap->graphicsView());
	this->updateActionStatus();
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

void GeoDataPointmap::showPolygonSelectedPoints(bool xOr)
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

	for (int i = 0; i < this->m_selectedVerticesGrid->GetNumberOfPoints(); i++) {
		double x[3];
		this->m_selectedVerticesGrid->GetPoint(i,x);
		double closestPoint[3];
		int subId;
		double pcoords[3];
		double weights[32];
		double dist;
		int inside = this->m_vtkPolygon->EvaluatePosition(x, closestPoint, subId, pcoords, dist, weights);
		if (inside || oldSelectedVertices.contains(i)) {
			//if( 1 == this->m_vtkPolygon->EvaluatePosition(x, closestPoint, subId, pcoords, dist, weights)){
			//if( 1 == this->m_vtkPolygon->PointInPolygon(x, ){
			vtkSmartPointer<vtkVertex> v = vtkSmartPointer<vtkVertex>::New();
			v->GetPointIds()->SetId(0,i);
			this->m_selectedVerticesGrid->InsertNextCell(v->GetCellType(), v->GetPointIds());
		}
	}
//		this->m_selectedVerticesGrid->BuildLinks();
	this->m_selectedVerticesGrid->Modified();
	this->enablePointSelectedActions(true);
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

void GeoDataPointmap::selectionModeBox(bool on)
{
	this->clearPointsSelection();

	if (on) {
		m_mouseEventMode = meSMBoxNotPossible;
	} else {
		m_mouseEventMode = meNormal;
	}
	updateActionStatus();
}

void GeoDataPointmap::selectionModePolygon(bool on)
{
	this->clearPointsSelection();

	if (on) {
		m_mouseEventMode = GeoDataPointmap::meSMPolygonPrepare;
	} else {
		m_mouseEventMode = meNormal;
	}
	updateActionStatus();
}

void GeoDataPointmap::updateActionStatus()
{
	this->m_selectionModePoint->setDisabled(true);
	this->m_selectionModePoint->setChecked(false);

	this->m_selectionModePolygon->setDisabled(true);
	this->m_selectionModePolygon->setChecked(false);

	this->m_addPointAction->setDisabled(true);
	this->m_addPointAction->setChecked(false);

	this->m_interpolatePointAction->setDisabled(true);
	this->m_interpolatePointAction->setChecked(false);

	this->m_addBreakLineAction->setDisabled(true);
	this->m_addBreakLineAction->setChecked(false);

	this->m_removeBreakLineAction->setDisabled(true);
	this->m_removeBreakLineAction->setChecked(false);

	this->m_removeAllBreakLinesAction->setDisabled(true);
	this->m_removeAllBreakLinesAction->setChecked(false);

	switch (m_mouseEventMode) {
	case meNormal:
	case meNormalWithShift:
		this->m_selectionModePoint->setEnabled(true);
		this->m_selectionModePoint->setChecked(true);

		this->m_selectionModePolygon->setEnabled(true);
		this->m_addPointAction->setEnabled(true);
		this->m_interpolatePointAction->setEnabled(true);
		this->m_addBreakLineAction->setEnabled(true);
		this->m_addBreakLineAction->setEnabled(true);
		this->m_removeBreakLineAction->setEnabled(true);
		this->m_removeAllBreakLinesAction->setEnabled(true);
		break;
	case meSMBox:
		break;
	case meSMBoxPrepare:
		break;
	case meSMPolygonPrepare:
	case meSMPolygon:
		this->m_selectionModePoint->setEnabled(false);

		this->m_selectionModePolygon->setEnabled(true);
		this->m_selectionModePolygon->setChecked(true);
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
		this->m_addBreakLineAction->setEnabled(true);
		this->m_addBreakLineAction->setChecked(true);
		break;
	case meBreakLineRemove:
	case meBreakLineRemoveNotPossible:
		this->m_removeBreakLineAction->setEnabled(true);
		this->m_removeBreakLineAction->setChecked(true);
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
	QVector2D worldPos(dx, dy);
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
		if (isVertexSelectable(worldPos) && (this->m_vtkInterpPolygon->GetNumberOfPoints() == 0)) {
			m_mouseEventMode = meSMAddPointPrepare;
		} else {
			m_mouseEventMode = meSMAddPoint;
		}
		break;
	case meSMAddPointPrepare:
		if (isVertexSelectable(worldPos) && (this->m_vtkInterpPolygon->GetNumberOfPoints() == 0)) {
			m_mouseEventMode = meSMAddPointPrepare;
		} else {
			m_mouseEventMode = meSMAddPoint;
		}
		break;
	case meBreakLineAdd:
	case meBreakLineAddNotPossible:
		m_mouseEventMode = meBreakLineAddNotPossible;
		m_selectedVertexId2 = this->m_vtkPointLocator->FindClosestPoint(tmppos);
		m_mouseEventMode = meBreakLineAdd;
		pushRenderCommand(new BreakLineAddPointCommand(true, m_selectedVertexId2, this));
		break;
	case meBreakLineRemove:
	case meBreakLineRemoveNotPossible:
		m_mouseEventMode = meBreakLineRemoveNotPossible;
		if (m_activeBreakLine != 0) {m_activeBreakLine->setActive(false);}
		m_activeBreakLine = 0;
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

bool GeoDataPointmap::isVertexSelectable(const QVector2D& pos)
{
	double point2[3] = {pos.x(), pos.y(), 0.0};
	m_selectedVertexId2 = this->m_vtkPointLocator->FindClosestPoint(point2);
	double point[3];
	this->m_vtkGrid->GetPoint(m_selectedVertexId2, point);
	this->m_selectedZPos = this->m_vtkGrid->GetPointData()->GetArray(VALUES)->GetTuple1(this->m_selectedVertexId2);
	QVector2D vertexPos(point[0], point[1]);
	double limitdist = graphicsView()->stdRadius(iRIC::nearRadius());
	double dist = (vertexPos - pos).length();
	return (dist < limitdist);
}

void GeoDataPointmap::updateMouseCursor(PreProcessorGraphicsViewInterface* v)
{
	switch (this->m_mouseEventMode) {
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
		v->setCursor(this->m_interpPointAddCursor);
		break;
	case meSMInterpCtrlPoint:
		//v->setCursor(this->m_interpPointCtrlAddCursor);
		v->setCursor(Qt::CrossCursor);
		break;
	case meSMInterpPoint:
		v->setCursor(Qt::ArrowCursor);
		break;
	case meSMAddPoint:
		v->setCursor(Qt::ArrowCursor);
		break;
	case meSMAddPointPrepare:
		v->setCursor(this->m_interpPointAddCursor);
		break;
	case meSMAddCtrlPoint:
		v->setCursor(Qt::CrossCursor);
		break;
	case meBreakLineAddNotPossible:
		v->setCursor(Qt::ArrowCursor);
		break;
	case meBreakLineAdd:
		v->setCursor(this->m_addCursor);
		break;
	case meBreakLineRemoveNotPossible:
		v->setCursor(Qt::ArrowCursor);
		break;
	case meBreakLineRemove:
		v->setCursor(this->m_removeCursor);
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
	this->clearPointsSelection();
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

	outs << this->m_selectedVerticesGrid->GetNumberOfCells() << endl;
	vtkPoints* points = this->m_vtkGrid->GetPoints();
	vtkDoubleArray* values = vtkDoubleArray::SafeDownCast(this->m_vtkGrid->GetPointData()->GetArray(VALUES));
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

const QPolygonF GeoDataPointmap::polygon()
{
	QPolygonF ret;
	vtkIdList* idlist = m_vtkPolygon->GetPointIds();
	vtkPoints* points = m_vtkPolygon->GetPoints();
	int vCount = idlist->GetNumberOfIds();
	for (int i = 0; i < vCount; ++i) {
		vtkIdType id = idlist->GetId(i);
		double* p = points->GetPoint(id);
		ret << QPointF(*p, *(p + 1));
	}
	// For QPolygonF, the start vertex and end vertex should be the same point,
	// to define a closed polygon.
	vtkIdType id = idlist->GetId(0);
	double* p = points->GetPoint(id);
	ret << QPointF(*p, *(p + 1));

	return ret;
}

void GeoDataPointmap::clearPointsSelection()
{
	m_selectedVerticesGrid->Reset();
	m_selectedVerticesGrid->Allocate(10, 10);
	m_selectedVerticesGrid->SetPoints(m_vtkGrid->GetPoints());
	m_selectedVerticesGrid->Modified();
	renderGraphicsView();
}

void GeoDataPointmap::clearNewPoints()
{
	m_newPoints->Reset();
	m_newPoints->Allocate(10, 10);
	m_newPoints->Modified();
}

void GeoDataPointmap::resetSelectionPolygon()
{
	int numpolypts = this->m_vtkPolygon->GetPoints()->GetNumberOfPoints();
	for (int i = 0; i < numpolypts; i++) {
		iRICUndoStack::instance().undo();
	}
	this->m_vtkPolygon->GetPoints()->Reset();
	this->updateShapeData();
}

void GeoDataPointmap::unwindSelectedInterp()
{
	int numIntPts = this->m_vtkInterpPolygon->GetNumberOfPoints();
	for (int i = 0; i < numIntPts; i++) {
		iRICUndoStack::instance().undo();
	}

}
void GeoDataPointmap::resetSelectedInterp()
{
	this->m_InterpLineGrid->Reset();
	this->m_vtkInterpValue->Reset();
	this->m_vtkInterpPolygon->GetPoints()->Reset();
	this->updateInterpShapeData();
	this->selectionModePoint(true);
	this->updateActionStatus();

}

void GeoDataPointmap::interpolatePoints(bool on)
{
	this->clearPointsSelection();

	if (on) {
		if (m_representation != GeoDataPointmapRepresentationDialog::Points) {
			// @todo message review is needed.
			int result = QMessageBox::information(preProcessorWindow(), tr("Information"), tr("When you interpolate points, you have to switch to show points. Do you want to switch to show points now?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
			if (result == QMessageBox::Yes) {
				m_representation = GeoDataPointmapRepresentationDialog::Points;
				updateRepresentation();
			} else {
				return;
			}
		}
		InformationDialog::information(preProcessorWindow(), tr("Information"), tr("To interpolate points, select existing points by mouse-clicking, use Alt to define path between known points, and finish by double-clicking or pressing return key."), "geodatapointmapinterpolatepoint");
		m_mouseEventMode = GeoDataPointmap::meSMInterpPoint;
	} else {
		this->unwindSelectedInterp();
		this->resetSelectedInterp();
		//m_mouseEventMode = GeoDataPointmap::meSMPointPrepare;
	}
	updateActionStatus();
	this->renderGraphicsView();

}

void GeoDataPointmap::addPoints(bool on)
{
	clearPointsSelection();
	if (on) {
		if (m_representation == GeoDataPointmapRepresentationDialog::Surface) {
			// @todo message review is needed.
			int result = QMessageBox::information(preProcessorWindow(), tr("Information"), tr("When you add points, you have to switch to show points. Do you want to switch to show points now?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
			if (result == QMessageBox::Yes) {
				m_representation = GeoDataPointmapRepresentationDialog::Points;
				updateRepresentation();
			} else {
				return;
			}
		}
		m_mouseEventMode = GeoDataPointmap::meSMAddPoint;
		InformationDialog::information(preProcessorWindow(), tr("Information"), tr("To add new points, select an existing point first. The value at that point will be used as the "
																	 "default value for new points. Then, click at any position you want to add points, and finish by double-clicking of pressing return key."), "geodatapointmapaddpoint");
	} else {
		this->unwindSelectedInterp();
		this->resetSelectedInterp();
	}
	updateActionStatus();
	renderGraphicsView();
}

void GeoDataPointmap::enablePointSelectedActions(bool val)
{
	this->m_editPointsAction->setEnabled(val);
	this->m_editPointsDeleteAction->setEnabled(val);
	this->m_editPointsExportAction->setEnabled(val);
	this->m_editPointsGreaterThanAction->setEnabled(val);
	this->m_editPointsLessThanAction->setEnabled(val);

}

vtkPolygon* GeoDataPointmap::getVtkPolygon() const
{
	return m_vtkPolygon;
}

vtkPolygon* GeoDataPointmap::getVtkInterpPolygon() const
{
	return m_vtkInterpPolygon;
}

vtkDoubleArray* GeoDataPointmap::getVtkInterpValue() const
{
	return m_vtkInterpValue;
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

void GeoDataPointmap::finishAddPoint()
{
	if (this->m_vtkInterpPolygon->GetNumberOfPoints() > 1) {
		GeoDataPointmapAddPointDialog* dialog = new GeoDataPointmapAddPointDialog(this, preProcessorWindow());
		if (dialog->exec() == QDialog::Accepted) {
			this->unwindSelectedInterp();
			pushRenderCommand(new AddPointsCommand(this, dialog));
			this->resetSelectedInterp();
		} else {
			this->unwindSelectedInterp();
			this->resetSelectedInterp();
		}
	} else {
		QMessageBox msgBox;
		msgBox.setText("Must have selected one known point and defined at least one location for added point");
		msgBox.setInformativeText("Would you like to retry or cancel the add points operation?");
		msgBox.setStandardButtons(QMessageBox::Retry | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Cancel);
		if (msgBox.exec() == QMessageBox::Cancel) {
			this->unwindSelectedInterp();
			this->resetSelectedInterp();
		} else {
			// No need to do anything here.
		}
	}
}

void GeoDataPointmap::finishInterpPoint()
{
	if (this->lastInterpPointKnown && this->m_vtkInterpPolygon->GetNumberOfPoints() > 1) {
		GeoDataPointmapInterpolatePoints* dialog = new GeoDataPointmapInterpolatePoints(this, preProcessorWindow());
		if (dialog->exec() == QDialog::Accepted) {
			this->unwindSelectedInterp();
			pushRenderCommand(new AddInterpolatePointsCommand(this,dialog));
			this->resetSelectedInterp();
		} else {
			this->unwindSelectedInterp();
			this->resetSelectedInterp();
		}
	} else {
		QMessageBox msgBox;
		msgBox.setText("Must have either 2 known points and last point must be known");
		msgBox.setInformativeText("Would you like to retry or cancel the interpolation?");
		msgBox.setStandardButtons(QMessageBox::Retry | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Cancel);
		if (msgBox.exec() == QMessageBox::Cancel) {
			this->unwindSelectedInterp();
			this->resetSelectedInterp();
		} else {
			// No need to do anything here.
		}
	}
}

void GeoDataPointmap::doApplyOffset(double x, double y)
{
	vtkPoints* points = this->m_vtkDelaunayedPolyData->GetPoints();
	vtkPoints* gridPoints = this->m_vtkGrid->GetPoints();
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
