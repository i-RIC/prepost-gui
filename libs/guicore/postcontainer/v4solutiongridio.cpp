#include "../grid/v4particles2d.h"
#include "../grid/v4particles3d.h"
#include "../grid/v4polydata2d.h"
#include "../grid/v4structured2dgrid.h"
#include "../grid/v4structured3dgrid.h"
#include "../grid/v4unstructured2dgrid.h"
#include "../misc/cgnsutil.h"
#include "v4solutiongrid.h"
#include "v4solutiongridio.h"

#include <geoio/polygonutil.h>
#include <guibase/vtkpointsetextended/vtkpolydataextended2d.h>
#include <guibase/vtkpointsetextended/vtkpolydataextended3d.h>

#include <h5cgnsbase.h>
#include <h5cgnsgridcoordinates.h>
#include <h5cgnsparticlegroupimagesolution.h>
#include <h5cgnsparticlegroupsolution.h>
#include <h5cgnsparticlesolution.h>
#include <h5cgnspolydatasolution.h>
#include <h5cgnszone.h>
#include <public/h5cgnsparticlegroupsolution_groupreader.h>
#include <public/h5cgnspolydatasolution_groupreader.h>
#include <iriclib.h>
#include <iriclib_errorcodes.h>

namespace {

const char IBC[] = "IBC";

} // namespace

v4SolutionGrid* v4SolutionGridIO::loadGrid(SolverDefinitionGridType* gridType, iRICLib::H5CgnsZone* zone, const QPointF& offset, int* ier)
{
	v4Grid* grid = nullptr;
	if (zone->base()->dimension() == 3) {
		grid = loadStructured3dGrid(zone, offset, ier);
	} else if (zone->base()->dimension() == 2) {
		if (zone->type() == iRICLib::H5CgnsZone::Type::Structured) {
			grid = loadStructured2dGrid(zone, offset, ier);
		} else {
			grid = loadUnstructured2dGrid(zone, offset, ier);
		}
	}
	return new v4SolutionGrid(gridType, grid);
}

void v4SolutionGridIO::loadGrid(v4SolutionGrid* grid, iRICLib::H5CgnsZone* zone, const QPointF& offset, int* ier)
{
	auto grid2 = grid->grid();
	grid2->clear();

	auto sGrid3d = dynamic_cast<v4Structured3dGrid*> (grid2);
	if (sGrid3d != nullptr) {loadStructured3dGrid(sGrid3d, zone, offset, ier);}

	auto sGrid2d = dynamic_cast<v4Structured2dGrid*> (grid2);
	if (sGrid2d != nullptr) {loadStructured2dGrid(sGrid2d, zone, offset, ier);}

	auto uGrid2d = dynamic_cast<v4Unstructured2dGrid*> (grid2);
	if (uGrid2d != nullptr) {loadUnstructured2dGrid(uGrid2d, zone, offset, ier);}
}

v4SolutionGrid* v4SolutionGridIO::loadParticles2d(SolverDefinitionGridType* gridType, iRICLib::H5CgnsZone* zone, const QPointF& offset, int* ier)
{
	if (! zone->particleSolutionExists()) {return nullptr;}

	auto pSol = zone->particleSolution();

	auto particles = new v4Particles2d();
	auto polyData = particles->vtkConcreteData()->concreteData();

	int numParticles;
	*ier = pSol->count(&numParticles);
	if (*ier != IRIC_NO_ERROR) {return nullptr;}

	std::vector<double> dataX, dataY;
	*ier = pSol->readCoordinatesX(&dataX);
	if (*ier != IRIC_NO_ERROR) {return nullptr;}
	*ier = pSol->readCoordinatesY(&dataY);
	if (*ier != IRIC_NO_ERROR) {return nullptr;}

	auto points = polyData->GetPoints();
	auto verts = vtkSmartPointer<vtkCellArray>::New();
	points->Allocate(dataX.size());
	verts->Allocate(dataX.size());
	for (int i = 0; i < numParticles; ++i) {
		points->InsertNextPoint(dataX[i] - offset.x(), dataY[i] - offset.y(), 0);
		vtkIdType pId = i;
		verts->InsertNextCell(1, &pId);
	}
	polyData->SetPoints(points);
	polyData->SetVerts(verts);
	particles->pointsModified();

	vtkPointData* pd = polyData->GetPointData();
	CgnsUtil::loadScalarData(pSol, pd);
	CgnsUtil::loadVectorData(pSol, pd);
	particles->vtkData()->updateValueRangeSet();

	return new v4SolutionGrid(gridType, particles);
}

std::map<std::string, v4SolutionGrid*> v4SolutionGridIO::loadParticleGroups2d(SolverDefinitionGridType* gridType, iRICLib::H5CgnsZone* zone, const QPointF& offset, int* ier)
{
	std::map<std::string, v4SolutionGrid*> empty;
	if (! zone->particleGroupSolutionExists()) {
		*ier = IRIC_NO_ERROR;
		return empty;
	}

	std::map<std::string, v4SolutionGrid*> ret;

	auto pSol = zone->particleGroupSolution();
	std::vector<std::string> groupNames;
	*ier = pSol->readGroupNames(&groupNames);
	if (*ier != IRIC_NO_ERROR) {return empty;}

	for (const auto& groupName : groupNames) {
		auto particles = new v4Particles2d();
		auto polyData = particles->vtkConcreteData()->concreteData();

		int numParticles;
		*ier = pSol->count(groupName, &numParticles);
		if (*ier != IRIC_NO_ERROR) {return empty;}

		std::vector<double> dataX, dataY;
		*ier = pSol->readCoordinatesX(groupName, &dataX);
		if (*ier != IRIC_NO_ERROR) {return empty;}
		*ier = pSol->readCoordinatesY(groupName, &dataY);
		if (*ier != IRIC_NO_ERROR) {return empty;}

		auto points = polyData->GetPoints();
		auto verts = vtkSmartPointer<vtkCellArray>::New();
		points->Allocate(dataX.size());
		verts->Allocate(dataX.size());
		for (int i = 0; i < numParticles; ++i) {
			points->InsertNextPoint(dataX[i] - offset.x(), dataY[i] - offset.y(), 0);
			vtkIdType pId = i;
			verts->InsertNextCell(1, &pId);
		}
		polyData->SetPoints(points);
		polyData->SetVerts(verts);
		particles->pointsModified();

		vtkPointData* pd = polyData->GetPointData();
		auto reader = pSol->groupReader(groupName);
		CgnsUtil::loadScalarData(&reader, pd);
		CgnsUtil::loadVectorData(&reader, pd);
		particles->vtkData()->updateValueRangeSet();

		ret.insert({groupName, new v4SolutionGrid(gridType, particles)});
	}
	return ret;
}

std::map<std::string, v4SolutionGrid*> v4SolutionGridIO::loadParticleGroupImage2d(SolverDefinitionGridType* gridType, iRICLib::H5CgnsZone* zone, const QPointF& offset, int* ier)
{
	std::map<std::string, v4SolutionGrid*> empty;
	if (! zone->particleGroupImageSolutionExists()) {
		*ier = IRIC_NO_ERROR;
		return empty;
	}

	std::map<std::string, v4SolutionGrid*> ret;

	auto pSol = zone->particleGroupImageSolution();
	std::vector<std::string> groupNames;
	*ier = pSol->readGroupNames(&groupNames);
	if (*ier != IRIC_NO_ERROR) {return empty;}

	for (const auto& groupName : groupNames) {
		auto particles = new v4Particles2d();
		auto polyData = particles->vtkConcreteData()->concreteData();

		int numParticles;
		*ier = pSol->count(groupName, &numParticles);
		if (*ier != IRIC_NO_ERROR) {return empty;}

		std::vector<double> dataX, dataY;
		*ier = pSol->readCoordinatesX(groupName, &dataX);
		if (*ier != IRIC_NO_ERROR) {return empty;}
		*ier = pSol->readCoordinatesY(groupName, &dataY);
		if (*ier != IRIC_NO_ERROR) {return empty;}

		auto points = polyData->GetPoints();
		auto verts = vtkSmartPointer<vtkCellArray>::New();
		points->Allocate(dataX.size());
		verts->Allocate(dataX.size());
		for (int i = 0; i < numParticles; ++i) {
			points->InsertNextPoint(dataX[i] - offset.x(), dataY[i] - offset.y(), 0);
			vtkIdType pId = i;
			verts->InsertNextCell(1, &pId);
		}
		polyData->SetPoints(points);
		polyData->SetVerts(verts);
		particles->pointsModified();

		vtkPointData* pd = polyData->GetPointData();
		std::vector<double> size, angle;
		*ier = pSol->readSize(groupName, &size);
		if (*ier != IRIC_NO_ERROR) {return empty;}
		*ier = pSol->readAngle(groupName, &angle);
		if (*ier != IRIC_NO_ERROR) {return empty;}

		CgnsUtil::addScalarDataT<double, vtkDoubleArray>(size, "size", pd);
		CgnsUtil::addScalarDataT<double, vtkDoubleArray>(angle, "angle", pd);
		particles->vtkData()->updateValueRangeSet();

		ret.insert({groupName, new v4SolutionGrid(gridType, particles)});
	}
	return ret;
}

v4SolutionGrid* v4SolutionGridIO::loadParticles3d(SolverDefinitionGridType* gridType, iRICLib::H5CgnsZone* zone, const QPointF& offset, int* ier)
{
	if (! zone->particleSolutionExists()) {return nullptr;}

	auto pSol = zone->particleSolution();

	auto particles = new v4Particles3d();
	auto polyData = particles->vtkConcreteData()->concreteData();

	int numParticles;
	*ier = pSol->count(&numParticles);
	if (*ier != IRIC_NO_ERROR) {return nullptr;}

	std::vector<double> dataX, dataY, dataZ;
	*ier = pSol->readCoordinatesX(&dataX);
	if (*ier != IRIC_NO_ERROR) {return nullptr;}
	*ier = pSol->readCoordinatesY(&dataY);
	if (*ier != IRIC_NO_ERROR) {return nullptr;}
	*ier = pSol->readCoordinatesZ(&dataZ);
	if (*ier != IRIC_NO_ERROR) {return nullptr;}

	auto points = polyData->GetPoints();
	auto verts = vtkSmartPointer<vtkCellArray>::New();
	points->Allocate(dataX.size());
	verts->Allocate(dataX.size());
	for (int i = 0; i < numParticles; ++i) {
		points->InsertNextPoint(dataX[i] - offset.x(), dataY[i] - offset.y(), dataZ[i]);
		vtkIdType pId = i;
		verts->InsertNextCell(1, &pId);
	}
	polyData->SetPoints(points);
	polyData->SetVerts(verts);
	particles->pointsModified();

	vtkPointData* pd = polyData->GetPointData();
	CgnsUtil::loadScalarData(pSol, pd);
	CgnsUtil::loadVectorData(pSol, pd);
	particles->vtkData()->updateValueRangeSet();

	return new v4SolutionGrid(gridType, particles);
}

std::map<std::string, v4SolutionGrid*> v4SolutionGridIO::loadParticleGroups3d(SolverDefinitionGridType* gridType, iRICLib::H5CgnsZone* zone, const QPointF& offset, int* ier)
{
	std::map<std::string, v4SolutionGrid*> empty;
	if (! zone->particleGroupSolutionExists()) {
		*ier = IRIC_NO_ERROR;
		return empty;
	}

	std::map<std::string, v4SolutionGrid*> ret;

	auto pSol = zone->particleGroupSolution();
	std::vector<std::string> groupNames;
	*ier = pSol->readGroupNames(&groupNames);
	if (*ier != IRIC_NO_ERROR) {return empty;}

	for (const auto& groupName : groupNames) {
		auto particles = new v4Particles3d();
		auto polyData = particles->vtkConcreteData()->concreteData();

		int numParticles;
		*ier = pSol->count(groupName, &numParticles);
		if (*ier != IRIC_NO_ERROR) {return empty;}

		std::vector<double> dataX, dataY, dataZ;
		*ier = pSol->readCoordinatesX(groupName, &dataX);
		if (*ier != IRIC_NO_ERROR) {return empty;}
		*ier = pSol->readCoordinatesY(groupName, &dataY);
		if (*ier != IRIC_NO_ERROR) {return empty;}
		*ier = pSol->readCoordinatesZ(groupName, &dataZ);
		if (*ier != IRIC_NO_ERROR) {return empty;}

		auto points = polyData->GetPoints();
		auto verts = vtkSmartPointer<vtkCellArray>::New();
		points->Allocate(dataX.size());
		verts->Allocate(dataX.size());
		for (int i = 0; i < numParticles; ++i) {
			points->InsertNextPoint(dataX[i] - offset.x(), dataY[i] - offset.y(), dataZ[i]);
			vtkIdType pId = i;
			verts->InsertNextCell(1, &pId);
		}
		polyData->SetPoints(points);
		polyData->SetVerts(verts);
		particles->pointsModified();

		vtkPointData* pd = polyData->GetPointData();
		auto reader = pSol->groupReader(groupName);
		CgnsUtil::loadScalarData(&reader, pd);
		CgnsUtil::loadVectorData(&reader, pd);
		particles->vtkData()->updateValueRangeSet();

		ret.insert({groupName, new v4SolutionGrid(gridType, particles)});
	}
	return ret;
}

std::map<std::string, v4SolutionGrid*> v4SolutionGridIO::loadPolyDataGroup2d(SolverDefinitionGridType* gridType, iRICLib::H5CgnsZone* zone, const QPointF& offset, int* ier)
{
	std::map<std::string, v4SolutionGrid*> empty;
	if (! zone->polyDataSolutionExists()) {
		*ier = IRIC_NO_ERROR;
		return empty;
	}

	std::map<std::string, v4SolutionGrid*> ret;

	auto pSol = zone->polyDataSolution();
	std::vector<std::string> groupNames;
	*ier = pSol->readGroupNames(&groupNames);
	if (*ier != IRIC_NO_ERROR) {return empty;}

	for (const auto& groupName : groupNames) {
		auto pd2d = new v4PolyData2d();
		auto polyData = pd2d->vtkConcreteData()->concreteData();

		std::vector<int> typeVec;
		std::vector<int> sizeVec;
		std::vector<double> coordXVec;
		std::vector<double> coordYVec;

		std::vector<int> cellCounts;
		std::vector<vtkIdType> cellIds;

		pSol->readTypes(groupName, &typeVec);
		pSol->readSizes(groupName, &sizeVec);
		pSol->readCoordinatesX(groupName, &coordXVec);
		pSol->readCoordinatesY(groupName, &coordYVec);

		cellCounts.reserve(typeVec.size());
		cellIds.reserve(typeVec.size() * 2);

		auto points = polyData->GetPoints();
		points->Allocate(coordXVec.size());

		auto lines = vtkSmartPointer<vtkCellArray>::New();
		lines->Allocate(typeVec.size());
		auto polys = vtkSmartPointer<vtkCellArray>::New();
		polys->Allocate(typeVec.size());

		int startIdx = 0;
		vtkIdType triIds[3];

		auto vtkPol = vtkSmartPointer<vtkPolygon>::New();
		auto idList = vtkSmartPointer<vtkIdList>::New();

		std::vector<unsigned int> indices;
		for (unsigned int i = 0; i < typeVec.size(); ++i) {
			int t = typeVec.at(i);
			int s = sizeVec.at(i);

			if (t == IRIC_POLYDATA_POLYGON) {
				QPolygonF pol;
				pol.reserve(s + 1);
				for (int j = 0; j < s; ++j) {
					QPointF point(coordXVec.at(startIdx + j) - offset.x(), coordYVec.at(startIdx + j) - offset.y());
					pol.push_back(point);
					points->InsertNextPoint(point.x(), point.y(), 0);
				}

				auto firstPoint = pol.first();
				auto lastPoint = pol.last();
				if (firstPoint != lastPoint) {
					pol.push_back(firstPoint);
				}

				bool allSame = true;
				for (int i = 1; i < pol.size(); ++i) {
					allSame = allSame && (pol.at(0) == pol.at(i));
				}

				indices.resize(0);
				if (! allSame) {
					PolygonUtil::triangulateVtk(pol, &indices, vtkPol, idList);
				}

				unsigned int pos = 0;
				int cellCount = 0;
				while (pos < indices.size()) {
					for (int j = 0; j < 3; ++j) {
						triIds[j] = indices.at(pos + j) + startIdx;
					}
					polys->InsertNextCell(3, triIds);
					cellIds.push_back(i);
					++ cellCount;
					pos += 3;
				}
				cellCounts.push_back(cellCount);
			} else {
				std::vector<vtkIdType> tmp_ids;
				tmp_ids.reserve(s);
				for (int j = 0; j < s; ++j) {
					points->InsertNextPoint(coordXVec.at(startIdx + j) - offset.x(), coordYVec.at(startIdx + j) - offset.y(), 0);
					tmp_ids.push_back(startIdx + j);
				}
				lines->InsertNextCell(tmp_ids.size(), tmp_ids.data());
				cellCounts.push_back(1);
				cellIds.push_back(i);
			}
			startIdx += s;
		}
		polyData->SetLines(lines);
		polyData->SetPolys(polys);

		// load values
		auto reader = pSol->groupReader(groupName);
		reader.setCellCounts(cellCounts);
		*ier = CgnsUtil::loadScalarData(&reader, polyData->GetCellData());
		if (*ier != IRIC_NO_ERROR) {return empty;}

		pd2d->setCellIds(cellIds);
		pd2d->pointsModified();
		pd2d->vtkData()->updateValueRangeSet();

		ret.insert({groupName, new v4SolutionGrid(gridType, pd2d)});
	}
	return ret;
}

v4Structured2dGrid* v4SolutionGridIO::loadStructured2dGrid(iRICLib::H5CgnsZone* zone, const QPointF& offset, int* ier)
{
	auto grid = new v4Structured2dGrid();

	auto size = zone->size();
	grid->setDimensions(size[0], size[1]);

	loadStructured2dGrid(grid, zone, offset, ier, true);

	if (*ier != IRIC_NO_ERROR) {
		delete grid;
		return nullptr;
	}

	return grid;
}

void v4SolutionGridIO::loadStructured2dGrid(v4Structured2dGrid* grid, iRICLib::H5CgnsZone* zone, const QPointF& offset, int* ier, bool forceLoadCoords)
{
	*ier = loadCoordinates2d(grid, zone, offset, forceLoadCoords);

	if (zone->nodeSolutionExists() && zone->nodeSolution() != nullptr) {
		*ier = CgnsUtil::loadScalarData(zone->nodeSolution(), grid->vtkData()->data()->GetPointData(), IBC);
		if (*ier != IRIC_NO_ERROR) {return;}

		*ier = CgnsUtil::loadVectorData(zone->nodeSolution(), grid->vtkData()->data()->GetPointData());
		if (*ier != IRIC_NO_ERROR) {return;}
	}
	if (zone->cellSolutionExists() && zone->cellSolution() != nullptr) {
		*ier = CgnsUtil::loadScalarData(zone->cellSolution(), grid->vtkData()->data()->GetCellData(), IBC);
		if (*ier != IRIC_NO_ERROR) {return;}
	}
	if (zone->iFaceSolutionExists() && zone->iFaceSolution() != nullptr) {
		*ier = CgnsUtil::loadScalarData(zone->iFaceSolution(), grid->vtkIEdgeData()->concreteData()->GetCellData(), IBC);
		if (*ier != IRIC_NO_ERROR) {return;}
	}
	if (zone->jFaceSolutionExists() && zone->jFaceSolution() != nullptr) {
		*ier = CgnsUtil::loadScalarData(zone->jFaceSolution(), grid->vtkJEdgeData()->concreteData()->GetCellData(), IBC);
		if (*ier != IRIC_NO_ERROR) {return;}
	}
	grid->vtkData()->updateValueRangeSet();
	grid->vtkIEdgeData()->updateValueRangeSet();
	grid->vtkJEdgeData()->updateValueRangeSet();
}

v4Unstructured2dGrid* v4SolutionGridIO::loadUnstructured2dGrid(iRICLib::H5CgnsZone* zone, const QPointF& offset, int* ier)
{
	auto grid = new v4Unstructured2dGrid();

	auto size = zone->size();
	int pointCount = size[0];

	auto points = grid->vtkData()->data()->GetPoints();
	for (int i = 0; i < pointCount; ++i) {
		points->InsertNextPoint(0, 0, 0);
	}

	std::vector<int> indices;
	*ier = zone->readTriangleElements(&indices);
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

	loadUnstructured2dGrid(grid, zone, offset, ier, true);

	if (*ier != IRIC_NO_ERROR) {
		delete grid;
		return nullptr;
	}

	return grid;
}

void v4SolutionGridIO::loadUnstructured2dGrid(v4Unstructured2dGrid* grid, iRICLib::H5CgnsZone* zone, const QPointF& offset, int* ier, bool forceLoadCoords) {
	*ier = loadCoordinates2d(grid, zone, offset, forceLoadCoords);

	if (zone->nodeSolutionExists() && zone->nodeSolution() != nullptr) {
		*ier = CgnsUtil::loadScalarData(zone->nodeSolution(), grid->vtkData()->data()->GetPointData(), IBC);
		if (*ier != IRIC_NO_ERROR) {return;}

		*ier = CgnsUtil::loadVectorData(zone->nodeSolution(), grid->vtkData()->data()->GetPointData());
		if (*ier != IRIC_NO_ERROR) {return;}
	}
	if (zone->cellSolutionExists() && zone->cellSolution() != nullptr) {
		*ier = CgnsUtil::loadScalarData(zone->cellSolution(), grid->vtkData()->data()->GetCellData(), IBC);
		if (*ier != IRIC_NO_ERROR) {return;}
	}
	grid->vtkData()->updateValueRangeSet();
}

v4Structured3dGrid* v4SolutionGridIO::loadStructured3dGrid(iRICLib::H5CgnsZone* zone, const QPointF& offset, int* ier)
{
	auto grid = new v4Structured3dGrid();

	auto size = zone->size();
	grid->setDimensions(size[0], size[1], size[2]);

	loadStructured3dGrid(grid, zone, offset, ier, true);

	if (*ier != IRIC_NO_ERROR) {
		delete grid;
		return nullptr;
	}

	return grid;
}

void v4SolutionGridIO::loadStructured3dGrid(v4Structured3dGrid* grid, iRICLib::H5CgnsZone* zone, const QPointF& offset, int* ier, bool forceLoadCoords)
{
	iRICLib::H5CgnsGridCoordinates* coords = nullptr;
	if (zone->gridCoordinatesForSolutionExists()) {
		coords = zone->gridCoordinatesForSolution();
	} else if (forceLoadCoords) {
		coords = zone->gridCoordinates();
	}

	if (coords != nullptr) {
		std::vector<double> xVec, yVec, zVec;
		*ier = coords->readCoordinatesX(&xVec);
		if (*ier != IRIC_NO_ERROR) {return;}
		*ier = coords->readCoordinatesY(&yVec);
		if (*ier != IRIC_NO_ERROR) {return;}
		*ier = coords->readCoordinatesZ(&zVec);
		if (*ier != IRIC_NO_ERROR) {return;}

		auto points = grid->vtkData()->data()->GetPoints();
		for (unsigned int i = 0; i < xVec.size(); ++i) {
			points->SetPoint(i, xVec[i] - offset.x(), yVec[i] - offset.y(), zVec[i]);
		}
		grid->pointsModified();
	}

	if (zone->nodeSolutionExists() && zone->nodeSolution() != nullptr) {
		*ier = CgnsUtil::loadScalarData(zone->nodeSolution(), grid->vtkData()->data()->GetPointData(), IBC);
		if (*ier != IRIC_NO_ERROR) {return;}

		*ier = CgnsUtil::loadVectorData(zone->nodeSolution(), grid->vtkData()->data()->GetPointData());
		if (*ier != IRIC_NO_ERROR) {return;}
	}
	if (zone->cellSolutionExists() && zone->cellSolution() != nullptr) {
		*ier = CgnsUtil::loadScalarData(zone->cellSolution(), grid->vtkData()->data()->GetCellData(), IBC);
		if (*ier != IRIC_NO_ERROR) {return;}
	}
	if (zone->iFaceSolutionExists() && zone->iFaceSolution() != nullptr) {
		*ier = CgnsUtil::loadScalarData(zone->iFaceSolution(), grid->vtkIFaceData()->concreteData()->GetCellData(), IBC);
		if (*ier != IRIC_NO_ERROR) {return;}
	}
	if (zone->jFaceSolutionExists() && zone->jFaceSolution() != nullptr) {
		*ier = CgnsUtil::loadScalarData(zone->jFaceSolution(), grid->vtkJFaceData()->concreteData()->GetCellData(), IBC);
		if (*ier != IRIC_NO_ERROR) {return;}
	}
	if (zone->kFaceSolutionExists() && zone->kFaceSolution() != nullptr) {
		*ier = CgnsUtil::loadScalarData(zone->kFaceSolution(), grid->vtkKFaceData()->concreteData()->GetCellData(), IBC);
		if (*ier != IRIC_NO_ERROR) {return;}
	}
	grid->vtkData()->updateValueRangeSet();
	grid->vtkIFaceData()->updateValueRangeSet();
	grid->vtkJFaceData()->updateValueRangeSet();
	grid->vtkKFaceData()->updateValueRangeSet();
}

int v4SolutionGridIO::loadCoordinates2d(v4Grid2d* grid, iRICLib::H5CgnsZone* zone, const QPointF& offset, bool forceLoadCoords)
{
	iRICLib::H5CgnsGridCoordinates* coords = nullptr;
	if (zone->gridCoordinatesForSolutionExists()) {
		coords = zone->gridCoordinatesForSolution();
	} else if (forceLoadCoords) {
		coords = zone->gridCoordinates();
	}

	if (coords != nullptr) {
		int ier;

		std::vector<double> xVec, yVec;
		ier = coords->readCoordinatesX(&xVec);
		if (ier != IRIC_NO_ERROR) {return ier;}
		ier = coords->readCoordinatesY(&yVec);
		if (ier != IRIC_NO_ERROR) {return ier;}

		for (unsigned int i = 0; i < xVec.size(); ++i) {
			grid->setPoint2d(i, QPointF(xVec[i] - offset.x(), yVec[i] - offset.y()));
		}
		grid->pointsModified();
	}

	return IRIC_NO_ERROR;
}

v4SolutionGridIO::v4SolutionGridIO()
{}
