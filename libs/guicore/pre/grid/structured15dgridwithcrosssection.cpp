#include "../../project/projectcgnsfile.h"
#include "../gridcond/base/gridattributecontainer.h"
#include "structured15dgrid/structured15dgridwithcrosssectioncrosssection.h"
#include "structured15dgridwithcrosssection.h"

#include <misc/stringtool.h>

#include <vtkLine.h>
#include <vtkVertex.h>

#include <QVector2D>

#include <cgnslib.h>
#include <iriclib.h>
#include <vector>

Structured15DGridWithCrossSection::Structured15DGridWithCrossSection(ProjectDataItem* parent) :
	Grid2D {vtkUnstructuredGrid::New(), SolverDefinitionGridType::gtNormal1_5DGridWithCrosssection, parent}
{
	init();
}

Structured15DGridWithCrossSection::Structured15DGridWithCrossSection(const std::string& zonename, ProjectDataItem* parent) :
	Grid2D {vtkUnstructuredGrid::New(), zonename, SolverDefinitionGridType::gtNormal1_5DGridWithCrosssection, parent}
{
	init();
}

Structured15DGridWithCrossSection::~Structured15DGridWithCrossSection()
{}

vtkUnstructuredGrid* Structured15DGridWithCrossSection::vtkGrid() const
{
	return dynamic_cast<vtkUnstructuredGrid*>(Grid::vtkGrid());
}

vtkUnstructuredGrid* Structured15DGridWithCrossSection::vertexGrid() const
{
	return m_vertexGrid;
}

unsigned int Structured15DGridWithCrossSection::vertexCount() const
{
	return m_vertexGrid->GetNumberOfPoints();
}

unsigned int Structured15DGridWithCrossSection::cellCount() const
{
	return nodeCount() - 1;
}

void Structured15DGridWithCrossSection::init()
{
	m_vertexGrid = vtkUnstructuredGrid::New();
}

QPointF Structured15DGridWithCrossSection::vertex(unsigned int index) const
{
	double points[3];
	vtkGrid()->GetPoints()->GetPoint(index, points);

	return QPointF(points[0], points[1]);
}

void Structured15DGridWithCrossSection::setVertex(unsigned int index, const QPointF& v)
{
	vtkGrid()->GetPoints()->SetPoint(index, v.x(), v.y(), 0.0);
}

void Structured15DGridWithCrossSection::setPoints(vtkPoints* points)
{
	vtkIdType num = points->GetNumberOfPoints();
	vtkUnstructuredGrid* edges = vtkGrid();

	edges->SetPoints(points);
	m_vertexGrid->SetPoints(points);

	// for edges
	vtkLine* line = vtkLine::New();
	for (vtkIdType i = 0; i < num - 1; i++) {
		line->GetPointIds()->InsertId(0, i);
		line->GetPointIds()->InsertId(1, i + 1);
		edges->InsertNextCell(line->GetCellType(), line->GetPointIds());
	}
	line->Delete();
	edges->BuildLinks();

	// for vertices
	vtkVertex* vertex = vtkVertex::New();
	for (vtkIdType i = 0; i < num; i++) {
		vertex->GetPointIds()->InsertId(0, i);
		m_vertexGrid->InsertNextCell(vertex->GetCellType(), vertex->GetPointIds());
	}
	vertex->Delete();
	m_vertexGrid->BuildLinks();
}

QList<Structured15DGridWithCrossSectionCrossSection*>& Structured15DGridWithCrossSection::crossSections()
{
	return m_crossSections;
}

const QList<Structured15DGridWithCrossSectionCrossSection*>& Structured15DGridWithCrossSection::crossSections() const
{
	return m_crossSections;
}

Structured15DGridWithCrossSectionCrossSection* Structured15DGridWithCrossSection::crossSections(int i)
{
	return m_crossSections.at(i);
}

void Structured15DGridWithCrossSection::setModified(bool modified)
{
	Grid::setModified(modified);
	if (modified) {
		m_vertexGrid->Modified();
	}
}

bool Structured15DGridWithCrossSection::loadFromCgnsFile(const int fn, int B, int Z)
{
	int ier;
	cgsize_t size[9];
	char buffer[ProjectCgnsFile::BUFFERLEN];
	ier = cg_zone_read(fn, B, Z, buffer, size);
	if (ier != 0) {return false;}
	int riversize = size[0];

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

	std::vector<double> dataX(riversize, 0);
	std::vector<double> dataY(riversize, 0);

	ier = cg_array_read_as(1, RealDouble, dataX.data());
	ier = cg_array_read_as(2, RealDouble, dataY.data());

	vtkPoints* points = vtkPoints::New();
	points->SetDataTypeToDouble();
	for (int i = 0; i < riversize; i++) {
		points->InsertNextPoint(dataX[i], dataY[i], 0);
	}
	setPoints(points);
	points->Delete();

	// Grid coordinates are loaded.
	// Next, grid related condition data is loaded.
	loadGridAttributes(fn, B, Z);

	// Grid related conditions are loaded.
	// Next, cross section data is loaded.
	for (int i = 1; i <= riversize; i++) {
		QString str = QString::number(i);
		Structured15DGridWithCrossSectionCrossSection* crossSection = new Structured15DGridWithCrossSectionCrossSection(str, this);
		crossSection->loadFromCgnsFile(fn, B, Z, i);
		m_crossSections << crossSection;
	}
	return true;
}

bool Structured15DGridWithCrossSection::saveToCgnsFile(const int fn, int B, const char* zonename)
{
	int ier;
	cgsize_t sizes[9];

	// Check whether the corresponding zone already exists.
	int zoneid = zoneId(zonename, fn, B, sizes);
	if (zoneid != 0) {
		// Zone already exists! remove this zone first!
		ier = cg_delete_node(zonename);
		if (ier != 0) {return false;}
	}
	// Now, create new zone.
	int num = vtkGrid()->GetNumberOfPoints();
	sizes[0] = num;
	sizes[1] = 1;
	sizes[2] = num - 1;
	sizes[3] = 0;
	sizes[4] = 0;
	sizes[5] = 0;
	cg_zone_write(fn, B, zonename, sizes, Structured, &zoneid);

	// save grid coordinates.
	int G;
	ier = cg_grid_write(fn, B, zoneid, "GridCoordinates", &G);
	if (ier != 0) {return false;}
	int C;
	// save coordinates.
	std::vector<double> dataX(num, 0);
	std::vector<double> dataY(num, 0);
	double points[3];

	for (int i = 0; i < num; i++) {
		vtkGrid()->GetPoints()->GetPoint(i, points);
		dataX[i] = points[0];
		dataY[i] = points[1];
	}
	ier = cg_coord_write(fn, B, zoneid, RealDouble, "CoordinateX", dataX.data(), &C);
	if (ier != 0) {return false;}
	ier = cg_coord_write(fn, B, zoneid, RealDouble, "CoordinateY", dataY.data(), &C);
	if (ier != 0) {return false;}

	// Grid coordinates are saved.
	// Next grid related condition data is saved.
	// Create "GridConditions" node under the zone node.
	saveGridAttributes(fn, B, zoneid);

	// Grid related conditions are saved.
	// Next cross section data is saved.
	cg_goto(fn, B, "Zone_t", zoneid, "end");
	cg_user_data_write("GridCrosssections");
	int index = 1;
	for (Structured15DGridWithCrossSectionCrossSection* xsec : m_crossSections) {
		xsec->saveToCgnsFile(fn, B, zoneid, index);
		index++;
	}
	return true;
}
