#include "v4inputgrid.h"
#include "v4inputgridio.h"
#include "../../grid/v4structured15dgridwithcrosssection.h"
#include "../../grid/v4structured15dgridwithcrosssectioncrosssection.h"
#include "../../grid/v4structured2dgrid.h"
#include "../../grid/v4unstructured2dgrid.h"
#include "../../solverdef/solverdefinitiongridtype.h"
#include "../gridcond/base/gridattributecontainer.h"

#include <iriclib_errorcodes.h>
#include <h5cgnsbase.h>
#include <h5cgnsgridattributes.h>
#include <h5cgnsgridcoordinates.h>
#include <h5cgnszone.h>
#include <h5groupcloser.h>
#include <h5util.h>

#include <QPointF>

v4InputGrid* v4InputGridIO::load(const iRICLib::H5CgnsZone& zone, SolverDefinitionGridType* gridType, const QPointF& offset, int* ier)
{
	v4Grid* grid = nullptr;
	if (zone.type() == iRICLib::H5CgnsZone::Type::Unstructured) {
		grid = loadUnstructured2dGrid(zone, offset, ier);
	} else {
		auto size = zone.size();
		if (size[1] == 1) {
			grid = loadStructured15DGridWithCrossSection(zone, offset, ier);
		} else {
			grid = loadStructured2dGrid(zone, offset, ier);
		}
	}
	if (*ier != IRIC_NO_ERROR) {return nullptr;}

	auto inputGrid = new v4InputGrid(gridType, grid);
	gridType->buildGridAttributes(inputGrid);
	inputGrid->allocateAttributes();

	for (auto att : inputGrid->attributes()) {
		*ier = att->loadFromCgnsFile(*zone.gridAttributes());
		if (*ier != IRIC_NO_ERROR) {return nullptr;}
	}
	inputGrid->grid()->vtkData()->updateValueRangeSet();

	*ier = IRIC_NO_ERROR;
	return inputGrid;
}

v4Structured2dGrid* v4InputGridIO::loadStructured2dGrid(const iRICLib::H5CgnsZone& zone, const QPointF& offset, int* ier)
{
	auto grid = new v4Structured2dGrid();

	auto size = zone.size();
	grid->setDimensions(size[0], size[1]);

	std::vector<double> xVec, yVec;
	auto coords = zone.gridCoordinates();

	*ier = coords->readCoordinatesX(&xVec);
	if (*ier != IRIC_NO_ERROR) {delete grid; return nullptr;}
	*ier = coords->readCoordinatesY(&yVec);
	if (*ier != IRIC_NO_ERROR) {delete grid; return nullptr;}

	for (unsigned int i = 0; i < xVec.size(); ++i) {
		grid->setPoint2d(i, QPointF(xVec[i] - offset.x(), yVec[i] - offset.y()));
	}
	grid->pointsModified();

	*ier = IRIC_NO_ERROR;
	return grid;
}

v4Unstructured2dGrid* v4InputGridIO::loadUnstructured2dGrid(const iRICLib::H5CgnsZone& zone, const QPointF& offset, int* ier)
{
	auto grid = new v4Unstructured2dGrid();

	auto size = zone.size();
	std::vector<double> xVec, yVec;
	auto coords = zone.gridCoordinates();

	*ier = coords->readCoordinatesX(&xVec);
	if (*ier != IRIC_NO_ERROR) {delete grid; return nullptr;}
	*ier = coords->readCoordinatesY(&yVec);
	if (*ier != IRIC_NO_ERROR) {delete grid; return nullptr;}

	auto points = grid->vtkData()->data()->GetPoints();

	for (unsigned int i = 0; i < xVec.size(); ++i) {
		points->InsertNextPoint(xVec[i] - offset.x(), yVec[i] - offset.y(), 0);
	}

	std::vector<int> indices;
	*ier = zone.readTriangleElements(&indices);
	if (*ier != IRIC_NO_ERROR) {delete grid; return nullptr;}

	unsigned int numCells = static_cast<unsigned int> (indices.size()) / 3;
	auto vtkGrid = grid->vtkConcreteData()->concreteData();
	vtkGrid->Allocate(numCells);

	auto triangle = vtkSmartPointer<vtkTriangle>::New();

	for (unsigned int i = 0; i < numCells; ++i) {
		int id0 = indices.at(i * 3 + 0) - 1;
		int id1 = indices.at(i * 3 + 1) - 1;
		int id2 = indices.at(i * 3 + 2) - 1;

		triangle->GetPointIds()->SetId(0, id0);
		triangle->GetPointIds()->SetId(1, id1);
		triangle->GetPointIds()->SetId(2, id2);
		vtkGrid->InsertNextCell(triangle->GetCellType(), triangle->GetPointIds());
	}
	vtkGrid->BuildLinks();

	*ier = IRIC_NO_ERROR;
	return grid;
}

v4Structured15dGridWithCrossSection* v4InputGridIO::loadStructured15DGridWithCrossSection(const iRICLib::H5CgnsZone& zone, const QPointF& offset, int* ier)
{
	auto grid = new v4Structured15dGridWithCrossSection();

	auto size = zone.size();
	std::vector<double> xVec, yVec;
	auto coords = zone.gridCoordinates();

	*ier = coords->readCoordinatesX(&xVec);
	if (*ier != IRIC_NO_ERROR) {delete grid; return nullptr;}
	*ier = coords->readCoordinatesY(&yVec);
	if (*ier != IRIC_NO_ERROR) {delete grid; return nullptr;}

	grid->setCrossSectionCount(static_cast<unsigned int>(xVec.size()));

	for (unsigned int i = 0; i < xVec.size(); ++i) {
		grid->setPoint2d(i, QPointF(xVec[i] - offset.x(), yVec[i] - offset.y()));
	}

	hid_t groupId;
	*ier = iRICLib::H5Util::openGroup(zone.groupId(), "GridCrosssections", iRICLib::H5Util::userDefinedDataLabel(), &groupId);
	if (*ier != IRIC_NO_ERROR) {delete grid; return nullptr;}

	iRICLib::H5GroupCloser groupCloser(groupId);

	for (unsigned int i = 0; i < xVec.size(); ++i) {
		auto cs = grid->crossSection(i);

		std::ostringstream ss;
		ss << "Crosssection" << (i + 1);
		std::vector<double> data;
		*ier = iRICLib::H5Util::readDataArrayValue(groupId, ss.str(), &data);
		if (*ier != IRIC_NO_ERROR) {delete grid; return nullptr;}

		auto count = data.size() / 2;
		for (unsigned int i = 0; i < count; ++i) {
			v4Structured15dGridWithCrossSectionCrossSection::Altitude alt;
			alt.position = data[i];
			alt.height = data[i + count];
			cs->altitudeInfo().push_back(alt);
		}
	}

	*ier = IRIC_NO_ERROR;
	return grid;
}

int v4InputGridIO::saveGrid(v4InputGrid* grid, iRICLib::H5CgnsBase* base, const std::string& zoneName, const QPointF& offset, iRICLib::H5CgnsZone** zone)
{
	v4Grid* g = grid->grid();
	int ier = IRIC_NO_ERROR;

	auto st2 = dynamic_cast<v4Structured2dGrid*> (g);
	if (st2 != nullptr) {
		ier = saveStructured2dGrid(st2, base, zoneName, offset, zone);
		if (ier != IRIC_NO_ERROR) {return ier;}
	}
	auto ust2 = dynamic_cast<v4Unstructured2dGrid*> (g);
	if (ust2 != nullptr) {
		ier = saveUnstructured2dGrid(ust2, base, zoneName, offset, zone);
		if (ier != IRIC_NO_ERROR) {return ier;}
	}
	auto st15 = dynamic_cast<v4Structured15dGridWithCrossSection*> (g);
	if (st15 != nullptr) {
		ier = saveStructured15DGridWithCrossSection(st15, base, zoneName, offset, zone);
		if (ier != IRIC_NO_ERROR) {return ier;}
	}

	for (auto att : grid->attributes()) {
		ier = att->saveToCgnsFile((*zone)->gridAttributes());
		if (ier != IRIC_NO_ERROR) {return ier;}
	}

	return IRIC_NO_ERROR;
}

int v4InputGridIO::saveStructured2dGrid(v4Structured2dGrid* grid, iRICLib::H5CgnsBase* base, const std::string& zoneName, const QPointF& offset, iRICLib::H5CgnsZone** z)
{
	int ier;
	std::vector<int> sizes;
	sizes.push_back(grid->dimensionI());
	sizes.push_back(grid->dimensionJ());
	sizes.push_back(grid->dimensionI() - 1);
	sizes.push_back(grid->dimensionJ() - 1);

	auto zone = base->createZone(zoneName, iRICLib::H5CgnsZone::Type::Structured, sizes);

	auto vtkGrid = grid->vtkData()->data();
	// save coordinates.
	std::vector<double> dataX(vtkGrid->GetNumberOfPoints(), 0);
	std::vector<double> dataY(vtkGrid->GetNumberOfPoints(), 0);

	auto points = vtkGrid->GetPoints();
	double p[3];

	for (vtkIdType i = 0; i < grid->nodeCount(); ++i) {
		points->GetPoint(i, p);
		dataX[i] = p[0] + offset.x();
		dataY[i] = p[1] + offset.y();
	}
	ier = zone->gridCoordinates()->writeCoordinatesX(dataX);
	if (ier != IRIC_NO_ERROR) {return ier;}

	ier = zone->gridCoordinates()->writeCoordinatesY(dataY);
	if (ier != IRIC_NO_ERROR) {return ier;}

	*z = zone;
	return IRIC_NO_ERROR;
}

int v4InputGridIO::saveUnstructured2dGrid(v4Unstructured2dGrid* grid, iRICLib::H5CgnsBase* base, const std::string& zoneName, const QPointF& offset, iRICLib::H5CgnsZone** z)
{
	int ier;
	std::vector<int> sizes;
	sizes.push_back(grid->nodeCount());
	sizes.push_back(grid->cellCount());

	auto zone = base->createZone(zoneName, iRICLib::H5CgnsZone::Type::Unstructured, sizes);

	auto vtkGrid = grid->vtkData()->data();
	// save coordinates.
	std::vector<double> dataX(vtkGrid->GetNumberOfPoints(), 0);
	std::vector<double> dataY(vtkGrid->GetNumberOfPoints(), 0);

	auto points = vtkGrid->GetPoints();
	double p[3];

	for (vtkIdType i = 0; i < grid->nodeCount(); ++i) {
		points->GetPoint(i, p);
		dataX[i] = p[0] + offset.x();
		dataY[i] = p[1] + offset.y();
	}
	ier = zone->gridCoordinates()->writeCoordinatesX(dataX);
	if (ier != IRIC_NO_ERROR) {return ier;}

	ier = zone->gridCoordinates()->writeCoordinatesY(dataY);
	if (ier != IRIC_NO_ERROR) {return ier;}

	// Save grid node connectivity data.
	// Unstructured grid that consists of triangles is supported.
	std::vector<int> indices(vtkGrid->GetNumberOfCells() * 3, 0);
	for (int i = 0; i < vtkGrid->GetNumberOfCells(); ++i) {
		vtkTriangle* tri = dynamic_cast<vtkTriangle*>(vtkGrid->GetCell(i));
		indices[i * 3]     = tri->GetPointId(0) + 1;
		indices[i * 3 + 1] = tri->GetPointId(1) + 1;
		indices[i * 3 + 2] = tri->GetPointId(2) + 1;
	}
	zone->writeTriangleElements(indices);

	*z = zone;
	return IRIC_NO_ERROR;
}

int v4InputGridIO::saveStructured15DGridWithCrossSection(v4Structured15dGridWithCrossSection* grid, iRICLib::H5CgnsBase* base, const std::string& zoneName, const QPointF& offset, iRICLib::H5CgnsZone** z)
{
	int ier;
	std::vector<int> sizes;
	sizes.push_back(grid->nodeCount());
	sizes.push_back(1);
	sizes.push_back(grid->nodeCount() - 1);
	sizes.push_back(0);

	auto zone = base->createZone(zoneName, iRICLib::H5CgnsZone::Type::Structured, sizes);

	auto vtkGrid = grid->vtkData()->data();
	// save coordinates.
	std::vector<double> dataX(vtkGrid->GetNumberOfPoints(), 0);
	std::vector<double> dataY(vtkGrid->GetNumberOfPoints(), 0);

	auto points = vtkGrid->GetPoints();
	double p[3];

	for (vtkIdType i = 0; i < grid->nodeCount(); ++i) {
		points->GetPoint(i, p);
		dataX[i] = p[0] + offset.x();
		dataY[i] = p[1] + offset.y();
	}
	ier = zone->gridCoordinates()->writeCoordinatesX(dataX);
	if (ier != IRIC_NO_ERROR) {return ier;}

	ier = zone->gridCoordinates()->writeCoordinatesY(dataY);
	if (ier != IRIC_NO_ERROR) {return ier;}

	// Next cross section data is saved.
	hid_t groupId;
	ier = iRICLib::H5Util::createUserDefinedDataGroup(zone->groupId(), "GridCrosssections", &groupId);
	if (ier != IRIC_NO_ERROR) {return ier;}

	iRICLib::H5GroupCloser groupCloser(groupId);

	for (int i = 0; i < grid->nodeCount(); ++i) {
		auto cs = grid->crossSection(i);

		std::ostringstream ss;
		ss << "Crosssection" << (i + 1);

		std::vector<hsize_t> dims(2);
		const auto& aInfo = cs->altitudeInfo();
		int size = static_cast<int> (aInfo.size());
		dims[0] = 2;
		dims[1] = size;

		std::vector<double> data(size * 2);
		for (int i = 0; i < size; ++i) {
			const auto& a = aInfo.at(i);
			data[i] = a.position;
			data[i + size] = a.height;
		}
		ier = iRICLib::H5Util::createDataArray(groupId, ss.str(), data, dims);
		if (ier != IRIC_NO_ERROR) {return ier;}
	}

	*z = zone;
	return IRIC_NO_ERROR;
}

v4InputGridIO::v4InputGridIO()
{}
