#include "../../project/projectcgnsfile.h"
#include "../gridcond/base/gridrelatedconditioncontainer.h"
#include "unstructured2dgrid.h"

#include <misc/stringtool.h>

#include <QSet>
#include <QString>

#include <vtkCellArray.h>
#include <vtkMaskPolyData.h>
#include <vtkPointData.h>
#include <vtkPolyDataAlgorithm.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTriangle.h>
#include <vtkTrivialProducer.h>

#include <cgnslib.h>
#include <iriclib.h>

#define ELEMNODENAME "Element"

Unstructured2DGrid::Unstructured2DGrid(ProjectDataItem* parent)
	: Grid2D(SolverDefinitionGridType::gtUnstructured2DGrid, parent)
{
	init();
}

Unstructured2DGrid::Unstructured2DGrid(const QString& zonename, ProjectDataItem* parent)
	: Grid2D(zonename, SolverDefinitionGridType::gtUnstructured2DGrid, parent)
{
	init();
}

void Unstructured2DGrid::init()
{
	m_vtkGrid = vtkUnstructuredGrid::New();
//	m_importerFactory = new Unstructured2DGridImporterFactory(this);
//	m_exporterFactory = new Unstructured2DGridExporterFactory(this);
}

unsigned int Unstructured2DGrid::vertexCount() const
{
	return m_vtkGrid->GetPoints()->GetNumberOfPoints();
}

const QVector2D Unstructured2DGrid::vertex(unsigned int index) const
{
	double v[3];
	m_vtkGrid->GetPoints()->GetPoint(index, v);
	return QVector2D(v[0], v[1]);
}

void Unstructured2DGrid::setVertex(unsigned int index, const QVector2D& v)
{
	m_vtkGrid->GetPoints()->SetPoint(index, v.x(), v.y(), 0);
}

bool Unstructured2DGrid::loadFromCgnsFile(const int fn, int B, int Z)
{
	int ier;
	cgsize_t size[9];
	char buffer[ProjectCgnsFile::BUFFERLEN];
	ier = cg_zone_read(fn, B, Z, buffer, size);

	if (ier != 0) {return false;}
	// for unstructured 2d grid, size[0] = NVertex, size[1] = NCell
	vtkUnstructuredGrid* grid = dynamic_cast<vtkUnstructuredGrid*>(m_vtkGrid);
	grid->Initialize();

	ier = cg_goto(fn, B, "Zone_t", Z, "GridCoordinates", 0, "end");
	if (ier != 0) {
		// grid data does not exists.
		return false;
	}
	// the first one must be X.
	DataType_t dType;
	int narrays;
	int dim;
	cgsize_t dimV[3];
	cg_narrays(&narrays);
	ier = cg_array_info(1, buffer, &dType, &dim, dimV);
	double* dataX = new double[size[0]];
	if (dType == RealSingle) {
		float* tmpFloat = new float[size[0]];
		ier = cg_array_read(1, tmpFloat);
		for (int i = 0; i < size[0]; ++i) {
			*(dataX + i) = *(tmpFloat + i);
		}
		delete[] tmpFloat;
	} else {
		ier = cg_array_read(1, dataX);
	}
	// the second one must be Y.
	ier = cg_array_info(2, buffer, &dType, &dim, dimV);
	double* dataY = new double[size[0]];
	if (dType == RealSingle) {
		float* tmpFloat = new float[size[0]];
		ier = cg_array_read(2, tmpFloat);
		for (int i = 0; i < size[0]; ++i) {
			*(dataY + i) = *(tmpFloat + i);
		}
		delete[] tmpFloat;
	} else {
		ier = cg_array_read(2, dataY);
	}
	vtkPoints* points = vtkPoints::New();
	points->SetDataTypeToDouble();
	for (int i = 0; i < size[0]; ++i) {
		points->InsertNextPoint(*(dataX + i), *(dataY + i), 0);
	}
	grid->SetPoints(points);
	points->Delete();
	delete[] dataX;
	delete[] dataY;

	// Grid coordinates are loaded.
	// load grid node connectivity data.
	// Unstructured grid that consists of triangles is supported.

	int numSections;
	cg_nsections(fn, B, Z, &numSections);
	for (int S = 1; S <= numSections; ++S) {
		ElementType_t eType;
		cgsize_t startIndex, endIndex;
		int nBndry, parent_flag;
		cg_section_read(fn, B, Z, S, buffer, &eType, &startIndex, &endIndex, &nBndry, &parent_flag);
		if (QString(buffer) == ELEMNODENAME) {
			// the target element node found!
			// eType must be TRI3.
			cgsize_t numCells;
			cg_ElementDataSize(fn, B, Z, S, &numCells);
			numCells = numCells / 3;
			cgsize_t* elements;
			elements = new cgsize_t[3 * numCells];
			cg_elements_read(fn, B, Z, S, elements, NULL);
			grid->Allocate(numCells);
			for (int i = 0; i < numCells; ++i) {
				vtkSmartPointer<vtkTriangle> triangle = vtkSmartPointer<vtkTriangle>::New();
				int id0 = *(elements + i * 3) - 1;
				int id1 = *(elements + i * 3 + 1) - 1;
				int id2 = *(elements + i * 3 + 2) - 1;
				triangle->GetPointIds()->SetId(0, id0);
				triangle->GetPointIds()->SetId(1, id1);
				triangle->GetPointIds()->SetId(2, id2);
				grid->InsertNextCell(triangle->GetCellType(), triangle->GetPointIds());
			}
			delete elements;
		}
	}
	loadGridRelatedConditions(fn, B, Z);

	grid->BuildLinks();
	return true;
}

bool Unstructured2DGrid::saveToCgnsFile(const int fn, int B, char* zonename)
{
	int ier;
	cgsize_t sizes[9];
	// Check whether the corresponsing zone already exists.
	int zoneid = zoneId(zonename, fn, B, sizes);
	if (zoneid != 0) {
		// Zone already exists! remove this zone first!
		ier = cg_delete_node(const_cast<char*>(iRIC::toStr(zonename).c_str()));
		if (ier != 0) {return false;}
	}
	// Now, create new zone.
	sizes[0] = m_vtkGrid->GetNumberOfPoints();
	sizes[1] = m_vtkGrid->GetNumberOfCells();
	sizes[2] = 0;
	sizes[3] = 0;
	sizes[4] = 0;
	sizes[5] = 0;
	ier = cg_zone_write(fn, B, iRIC::toStr(zonename).c_str(), sizes, Unstructured, &zoneid);
	if (ier != 0) {return false;}

	// save grid coordinates.
	int G;
	ier = cg_grid_write(fn, B, zoneid, "GridCoordinates", &G);
	if (ier != 0) {return false;}
	int C;
	// save coordinates.
	double* dataX = new double[m_vtkGrid->GetNumberOfPoints()];
	double* dataY = new double[m_vtkGrid->GetNumberOfPoints()];
	double points[3];

	for (int i = 0; i < m_vtkGrid->GetNumberOfPoints(); ++i) {
		m_vtkGrid->GetPoints()->GetPoint(i, points);
		*(dataX + i) = points[0];
		*(dataY + i) = points[1];
	}
	ier = cg_coord_write(fn, B, zoneid, RealDouble, "CoordinateX", dataX, &C);
	if (ier != 0) {return false;}
	ier = cg_coord_write(fn, B, zoneid, RealDouble, "CoordinateY", dataY, &C);
	if (ier != 0) {return false;}

	delete[] dataX;
	delete[] dataY;

	// Grid coordinates are saved.
	// Save grid node connectivity data.
	// Unstructured grid that consists of triangles is supported.

	cgsize_t* elements;
	elements = new cgsize_t[3 * m_vtkGrid->GetNumberOfCells()];
	for (int i = 0; i < m_vtkGrid->GetNumberOfCells(); ++i) {
		vtkTriangle* tri = dynamic_cast<vtkTriangle*>(m_vtkGrid->GetCell(i));
		*(elements + i * 3)     = tri->GetPointId(0) + 1;
		*(elements + i * 3 + 1) = tri->GetPointId(1) + 1;
		*(elements + i * 3 + 2) = tri->GetPointId(2) + 1;
	}
	int startIndex = 1;
	int endIndex = startIndex + m_vtkGrid->GetNumberOfCells() - 1;
	int S;
	ier = cg_section_write(fn, B, zoneid, ELEMNODENAME, TRI_3, startIndex, endIndex, 0, elements, &S);
	if (ier != 0) {return false;}

	// Next grid related condition data is saved.
	// Create "GridConditions" node under the zone node.
	saveGridRelatedConditions(fn, B, zoneid);
	return true;
}

void Unstructured2DGrid::updateSimplifiedGrid(double xmin, double xmax, double ymin, double ymax)
{
	Grid::updateSimplifiedGrid(xmin, xmax, ymin, ymax);
	setupIndexArray();
}

void Unstructured2DGrid::setupIndexArray()
{
	if (m_vtkFilteredCellsAlgorithm == nullptr) {return;}

	vtkAlgorithm* tmpalgo = m_vtkFilteredCellsAlgorithm;
	vtkPolyDataAlgorithm* algo = dynamic_cast<vtkPolyDataAlgorithm*>(tmpalgo);
	algo->Update();
	vtkSmartPointer<vtkPolyData> filteredCells = algo->GetOutput();
	int ccounts2 = filteredCells->GetNumberOfCells();
	vtkSmartPointer<vtkPolyData> tmpIndexPolyData;
	if (ccounts2 > MAX_DRAWINDEXCOUNT) {
		vtkSmartPointer<vtkMaskPolyData> maskPoly2 = vtkSmartPointer<vtkMaskPolyData>::New();
		int ratio = static_cast<int>(ccounts2 / MAX_DRAWINDEXCOUNT);
		if (ratio == 1) {ratio = 2;}
		maskPoly2->SetOnRatio(ratio);
		maskPoly2->SetInputData(filteredCells);
		maskPoly2->Update();
		tmpIndexPolyData = maskPoly2->GetOutput();
	} else {
		tmpIndexPolyData = filteredCells;
	}


	vtkSmartPointer<vtkPolyData> filteredIndexGrid = vtkSmartPointer<vtkPolyData>::New();
	vtkSmartPointer<vtkPoints> igPoints = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> ca = vtkSmartPointer<vtkCellArray>::New();
	vtkSmartPointer<vtkStringArray> sa = vtkSmartPointer<vtkStringArray>::New();
	sa->SetName(Grid::LABEL_NAME);
	vtkIdType cellid = 0;
	double tmpp[3];
	QString label("(%1)");
	QSet<vtkIdType> indices;

	tmpIndexPolyData->BuildCells();
	for (int i = 0; i < tmpIndexPolyData->GetNumberOfCells(); ++i) {
		vtkCell* cell = tmpIndexPolyData->GetCell(i);
		for (int j = 0; j < cell->GetNumberOfPoints(); ++j) {
			vtkIdType vid = cell->GetPointId(j);
			if (! indices.contains(vid)) {
				m_vtkGrid->GetPoint(vid, tmpp);
				igPoints->InsertNextPoint(tmpp);
				ca->InsertNextCell(1, &cellid);
				sa->InsertNextValue(iRIC::toStr(label.arg(vid + 1)));
				++ cellid;
				indices.insert(vid);
			}
		}
	}
	filteredIndexGrid->SetPoints(igPoints);
	filteredIndexGrid->SetVerts(ca);
	filteredIndexGrid->GetPointData()->AddArray(sa);

	vtkSmartPointer<vtkTrivialProducer> prod = vtkSmartPointer<vtkTrivialProducer>::New();
	prod->SetOutput(filteredIndexGrid);
	m_vtkFilteredIndexGridAlgorithm = prod;
}
