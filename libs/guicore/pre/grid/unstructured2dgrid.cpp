#include "../../project/projectcgnsfile.h"
#include "../gridcond/base/gridattributecontainer.h"
#include "unstructured2dgrid.h"

#include <misc/stringtool.h>

#include <QPointF>
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
#include <vector>

#define ELEMNODENAME "Element"

Unstructured2DGrid::Unstructured2DGrid(ProjectDataItem* parent) :
	Unstructured2DGrid("", parent)
{}

Unstructured2DGrid::Unstructured2DGrid(const std::string& zonename, ProjectDataItem* parent) :
	Grid2D {vtkUnstructuredGrid::New(), zonename, SolverDefinitionGridType::gtUnstructured2DGrid, parent}
{}

Unstructured2DGrid::~Unstructured2DGrid()
{}

vtkUnstructuredGrid* Unstructured2DGrid::vtkGrid() const
{
	return dynamic_cast<vtkUnstructuredGrid*>(Grid::vtkGrid());
}

unsigned int Unstructured2DGrid::vertexCount() const
{
	return vtkGrid()->GetPoints()->GetNumberOfPoints();
}

unsigned int Unstructured2DGrid::cellCount() const
{
	return vtkGrid()->GetNumberOfCells();
}

QPointF Unstructured2DGrid::vertex(unsigned int index) const
{
	double v[3];
	vtkGrid()->GetPoints()->GetPoint(index, v);
	return QPointF(v[0], v[1]);
}

void Unstructured2DGrid::setVertex(unsigned int index, const QPointF& v)
{
	vtkGrid()->GetPoints()->SetPoint(index, v.x(), v.y(), 0);
}

bool Unstructured2DGrid::loadFromCgnsFile(const int fn, int B, int Z)
{
	int ier;
	cgsize_t size[9];
	char buffer[ProjectCgnsFile::BUFFERLEN];
	ier = cg_zone_read(fn, B, Z, buffer, size);

	if (ier != 0) {return false;}
	// for unstructured 2d grid, size[0] = NVertex, size[1] = NCell
	vtkUnstructuredGrid* grid = dynamic_cast<vtkUnstructuredGrid*>(vtkGrid());
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
	std::vector<double> dataX(size[0], 0);
	std::vector<double> dataY(size[0], 0);
	ier = cg_array_read_as(1, RealDouble, dataX.data());
	ier = cg_array_read_as(2, RealDouble, dataY.data());

	vtkPoints* points = vtkPoints::New();
	points->SetDataTypeToDouble();
	auto offset = this->offset();
	for (int i = 0; i < size[0]; ++i) {
		points->InsertNextPoint(dataX[i] - offset.x(), dataY[i] - offset.y(), 0);
	}
	grid->SetPoints(points);
	points->Delete();

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
			std::vector<cgsize_t> elements(3 * numCells, 0);
			cg_elements_read(fn, B, Z, S, elements.data(), NULL);
			grid->Allocate(numCells);
			for (int i = 0; i < numCells; ++i) {
				vtkSmartPointer<vtkTriangle> triangle = vtkSmartPointer<vtkTriangle>::New();
				int id0 = elements[i * 3] - 1;
				int id1 = elements[i * 3 + 1] - 1;
				int id2 = elements[i * 3 + 2] - 1;
				triangle->GetPointIds()->SetId(0, id0);
				triangle->GetPointIds()->SetId(1, id1);
				triangle->GetPointIds()->SetId(2, id2);
				grid->InsertNextCell(triangle->GetCellType(), triangle->GetPointIds());
			}
		}
	}
	loadGridAttributes(fn, B, Z);

	grid->BuildLinks();
	return true;
}

bool Unstructured2DGrid::saveToCgnsFile(const int fn, int B, const char* zonename)
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
	sizes[0] = vtkGrid()->GetNumberOfPoints();
	sizes[1] = vtkGrid()->GetNumberOfCells();
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
	std::vector<double> dataX(vtkGrid()->GetNumberOfPoints(), 0);
	std::vector<double> dataY(vtkGrid()->GetNumberOfPoints(), 0);
	auto offset = this->offset();
	double points[3];

	for (int i = 0; i < vtkGrid()->GetNumberOfPoints(); ++i) {
		vtkGrid()->GetPoints()->GetPoint(i, points);
		dataX[i] = points[0] + offset.x();
		dataY[i] = points[1] + offset.y();
	}
	ier = cg_coord_write(fn, B, zoneid, RealDouble, "CoordinateX", dataX.data(), &C);
	if (ier != 0) {return false;}
	ier = cg_coord_write(fn, B, zoneid, RealDouble, "CoordinateY", dataY.data(), &C);
	if (ier != 0) {return false;}

	// Grid coordinates are saved.
	// Save grid node connectivity data.
	// Unstructured grid that consists of triangles is supported.

	std::vector<cgsize_t> elements(3 * vtkGrid()->GetNumberOfCells());
	for (int i = 0; i < vtkGrid()->GetNumberOfCells(); ++i) {
		vtkTriangle* tri = dynamic_cast<vtkTriangle*>(vtkGrid()->GetCell(i));
		elements[i * 3]     = tri->GetPointId(0) + 1;
		elements[i * 3 + 1] = tri->GetPointId(1) + 1;
		elements[i * 3 + 2] = tri->GetPointId(2) + 1;
	}
	int startIndex = 1;
	int endIndex = startIndex + vtkGrid()->GetNumberOfCells() - 1;
	int S;
	ier = cg_section_write(fn, B, zoneid, ELEMNODENAME, TRI_3, startIndex, endIndex, 0, elements.data(), &S);
	if (ier != 0) {return false;}

	// Next grid related condition data is saved.
	// Create "GridConditions" node under the zone node.
	saveGridAttributes(fn, B, zoneid);
	return true;
}

void Unstructured2DGrid::updateSimplifiedGrid(double xmin, double xmax, double ymin, double ymax)
{
	Grid::updateSimplifiedGrid(xmin, xmax, ymin, ymax);
	setupIndexArray();
}

vtkAlgorithm* Unstructured2DGrid::vtkFilteredIndexGridAlgorithm() const
{
	return m_vtkFilteredIndexGridAlgorithm;
}

void Unstructured2DGrid::setupIndexArray()
{
	if (vtkFilteredCellsAlgorithm() == nullptr) {return;}

	vtkAlgorithm* tmpalgo = vtkFilteredCellsAlgorithm();
	vtkPolyDataAlgorithm* algo = dynamic_cast<vtkPolyDataAlgorithm*>(tmpalgo);
	algo->Update();
	vtkSmartPointer<vtkPolyData> filteredCells = algo->GetOutput();
	int ccounts2 = filteredCells->GetNumberOfCells();
	vtkSmartPointer<vtkPolyData> tmpIndexPolyData;
	bool cullEnable;
	int cullCellLimit, cullIndexLimit;
	getCullSetting(&cullEnable, &cullCellLimit, &cullIndexLimit);
	if (cullEnable && ccounts2 > cullIndexLimit){
		vtkSmartPointer<vtkMaskPolyData> maskPoly2 = vtkSmartPointer<vtkMaskPolyData>::New();
		int ratio = static_cast<int>(ccounts2 / cullIndexLimit);
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
				vtkGrid()->GetPoint(vid, tmpp);
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
