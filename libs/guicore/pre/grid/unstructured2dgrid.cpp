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

#include <h5cgnsbase.h>
#include <h5cgnsgridcoordinates.h>
#include <h5cgnszone.h>
#include <iriclib_errorcodes.h>

#include <vector>

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

int Unstructured2DGrid::loadFromCgnsFile(const iRICLib::H5CgnsZone& zone)
{
	auto size = zone.size();

	auto grid = dynamic_cast<vtkUnstructuredGrid*> (vtkGrid());
	grid->Initialize();

	std::vector<double> xvec, yvec;
	int ier = 0;

	ier = zone.gridCoordinates()->readCoordinatesX(&xvec);
	if (ier != IRIC_NO_ERROR) {return ier;}

	ier = zone.gridCoordinates()->readCoordinatesY(&yvec);
	if (ier != IRIC_NO_ERROR) {return ier;}

	auto points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();
	auto offset = this->offset();
	for (unsigned int i = 0; i < xvec.size(); ++i) {
		points->InsertNextPoint(xvec[i] - offset.x(), yvec[i] - offset.y(), 0);
	}
	grid->SetPoints(points);

	// load connectivity data
	std::vector<int> indices;
	ier = zone.readTriangleElements(&indices);
	if (ier != IRIC_NO_ERROR) {return ier;}

	int numCells = static_cast<int> (indices.size()) / 3;
	grid->Allocate(numCells);
	for (unsigned int i = 0; i < numCells; ++i) {
		int id0 = indices.at(i * 3 + 0) - 1;
		int id1 = indices.at(i * 3 + 1) - 1;
		int id2 = indices.at(i * 3 + 2) - 1;

		auto triangle = vtkSmartPointer<vtkTriangle>::New();
		triangle->GetPointIds()->SetId(0, id0);
		triangle->GetPointIds()->SetId(1, id1);
		triangle->GetPointIds()->SetId(2, id2);
		grid->InsertNextCell(triangle->GetCellType(), triangle->GetPointIds());
	}

	ier = loadGridAttributes(*(zone.gridAttributes()));
	if (ier != IRIC_NO_ERROR) {return ier;}

	grid->BuildLinks();

	return IRIC_NO_ERROR;;
}

int Unstructured2DGrid::saveToCgnsFile(iRICLib::H5CgnsBase* base, const std::string& zoneName)
{
	int ier;
	std::vector<int> sizes;
	sizes.push_back(vtkGrid()->GetNumberOfPoints());
	sizes.push_back(vtkGrid()->GetNumberOfCells());

	auto zone = base->createZone(zoneName, iRICLib::H5CgnsZone::Type::Unstructured, sizes);

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
	ier = zone->gridCoordinates()->writeCoordinatesX(dataX);
	if (ier != IRIC_NO_ERROR) {return ier;}

	ier = zone->gridCoordinates()->writeCoordinatesY(dataY);
	if (ier != IRIC_NO_ERROR) {return ier;}

	// Save grid node connectivity data.
	// Unstructured grid that consists of triangles is supported.
	std::vector<int> indices(vtkGrid()->GetNumberOfCells() * 3, 0);
	for (int i = 0; i < vtkGrid()->GetNumberOfCells(); ++i) {
		vtkTriangle* tri = dynamic_cast<vtkTriangle*>(vtkGrid()->GetCell(i));
		indices[i * 3]     = tri->GetPointId(0) + 1;
		indices[i * 3 + 1] = tri->GetPointId(1) + 1;
		indices[i * 3 + 2] = tri->GetPointId(2) + 1;
	}
	zone->writeTriangleElements(indices);

	ier = saveGridAttributes(zone->gridAttributes());
	if (ier != IRIC_NO_ERROR) {return ier;}

	return IRIC_NO_ERROR;
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
