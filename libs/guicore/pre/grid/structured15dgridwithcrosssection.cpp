#include "../../project/projectcgnsfile.h"
#include "../gridcond/base/gridattributecontainer.h"
#include "structured15dgrid/structured15dgridwithcrosssectioncrosssection.h"
#include "structured15dgridwithcrosssection.h"

#include <misc/stringtool.h>

#include <vtkLine.h>
#include <vtkSmartPointer.h>
#include <vtkVertex.h>

#include <QVector2D>

#include <h5cgnsbase.h>
#include <h5cgnsgridcoordinates.h>
#include <h5cgnszone.h>
#include <h5groupcloser.h>
#include <h5util.h>
#include <iriclib_errorcodes.h>

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

int Structured15DGridWithCrossSection::loadFromCgnsFile(const iRICLib::H5CgnsZone& zone)
{
	auto size = zone.size();

	int riversize = size[0];

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
	setPoints(points);

	loadGridAttributes(*(zone.gridAttributes()));

	// load cross section data
	hid_t groupId;
	ier = iRICLib::H5Util::openGroup(zone.groupId(), "GridCrosssections", iRICLib::H5Util::userDefinedDataLabel(), &groupId);
	if (ier != IRIC_NO_ERROR) {return ier;}

	iRICLib::H5GroupCloser groupCloser(groupId);

	for (int i = 1; i <= riversize; i++) {
		QString str = QString::number(i);
		auto cs = new Structured15DGridWithCrossSectionCrossSection(str, this);
		cs->loadFromCgnsFile(groupId, i);
		m_crossSections.push_back(cs);
	}
	return true;
}

int Structured15DGridWithCrossSection::saveToCgnsFile(iRICLib::H5CgnsBase* base, const std::string& zoneName)
{
	int ier;
	std::vector<int> sizes;
	int num = vtkGrid()->GetNumberOfPoints();
	sizes.push_back(num);
	sizes.push_back(1);
	sizes.push_back(num - 1);
	sizes.push_back(0);

	auto zone = base->createZone(zoneName, iRICLib::H5CgnsZone::Type::Structured, sizes);

	// save coordinates.
	std::vector<double> dataX(num, 0);
	std::vector<double> dataY(num, 0);
	double points[3];

	for (int i = 0; i < num; i++) {
		vtkGrid()->GetPoints()->GetPoint(i, points);
		dataX[i] = points[0];
		dataY[i] = points[1];
	}

	ier = zone->gridCoordinates()->writeCoordinatesX(dataX);
	if (ier != IRIC_NO_ERROR) {return ier;}

	ier = zone->gridCoordinates()->writeCoordinatesY(dataY);
	if (ier != IRIC_NO_ERROR) {return ier;}

	ier = saveGridAttributes(zone->gridAttributes());
	if (ier != IRIC_NO_ERROR) {return ier;}

	// Next cross section data is saved.
	hid_t groupId;
	ier = iRICLib::H5Util::createUserDefinedDataGroup(zone->groupId(), "GridCrosssections", &groupId);
	if (ier != IRIC_NO_ERROR) {return ier;}

	iRICLib::H5GroupCloser groupCloser(groupId);

	int index = 1;
	for (auto xsec : m_crossSections) {
		ier = xsec->saveToCgnsFile(groupId, index);
		if (ier != IRIC_NO_ERROR) {return ier;}

		index++;
	}
	return true;
}
