#include "../misc/cgnsutil.h"
#include "../pre/grid/structured2dgrid.h"
#include "../project/projectcgnsfile.h"
#include "../project/projectdata.h"
#include "../project/projectmainfile.h"
#include "../solverdef/solverdefinitiongridattributeinteger.h"
#include "../solverdef/solverdefinitiongridattributereal.h"
#include "../solverdef/solverdefinitiongridtype.h"
#include "postcalculatedresult.h"
#include "postsolutioninfo.h"
#include "postzonedatacontainer.h"
#include "private/postzonedatacontainer_particlegrouploader.h"
#include "private/postzonedatacontainer_particleloader.h"
#include "private/postzonedatacontainer_polydataloader.h"

#include <cs/coordinatesystem.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guibase/vtktool/vtkpointsetregionandcellsizefilter.h>
#include <misc/filesystemfunction.h>
#include <misc/rectregion.h>
#include <misc/stringtool.h>

#include <QDomNode>
#include <QLineF>
#include <QPointF>
#include <QTextStream>
#include <QXmlStreamWriter>

#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkDataSetAttributes.h>
#include <vtkDoubleArray.h>
#include <vtkExtractGrid.h>
#include <vtkFloatArray.h>
#include <vtkGeometryFilter.h>
#include <vtkIntArray.h>
#include <vtkPointData.h>
#include <vtkQuad.h>
#include <vtkSmartPointer.h>
#include <vtkStaticPointLocator.h>
#include <vtkStaticPointLocator2D.h>
#include <vtkStringArray.h>
#include <vtkStructuredGrid.h>
#include <vtkTriangle.h>
#include <vtkUnstructuredGrid.h>

#include <ogr_spatialref.h>
#include <shapefil.h>

#include <h5cgnsbase.h>
#include <h5cgnsgridattributes.h>
#include <h5cgnsgridcoordinates.h>
#include <h5cgnszone.h>
#include <iriclib_errorcodes.h>

#include <sstream>
#include <vector>

#define ELEMNODENAME "Element"

// namespace for local funcitions
namespace {

vtkDoubleArray* buildDoubleArray(const std::string& name, vtkIdType size)
{
	vtkDoubleArray* ret = vtkDoubleArray::New();
	ret->SetName(name.c_str());
	ret->Allocate(size);
	for (vtkIdType i = 0; i < size; ++i) {
		ret->InsertNextValue(0);
	}
	return ret;
}

} // namespace

const QString PostZoneDataContainer::labelName {"_LABEL"};
const QString PostZoneDataContainer::IBC {"IBC"};
const double PostZoneDataContainer::IBCLimit {0.99};

PostZoneDataContainer::PostZoneDataContainer(const std::string& zoneName, SolverDefinitionGridType* gridtype, PostSolutionInfo* parent) :
	PostDataContainer {parent},
	m_gridType {gridtype},
	m_data {nullptr},
	m_edgeIData {nullptr},
	m_edgeJData {nullptr},
	m_iFaceData {nullptr},
	m_jFaceData {nullptr},
	m_particleData {nullptr},
	m_particleGroupMap {},
	m_polyDataMap {},
	m_polyDataCellIdsMap {},
	m_calculatedResults {},
	m_pointLocator {nullptr},
	m_zoneName (zoneName),
	m_size {},
	m_loadOK {false},
	m_loadedOnce {false}
{}

PostZoneDataContainer::~PostZoneDataContainer()
{
	delete m_data;
	delete m_edgeIData;
	delete m_edgeJData;
	delete m_iFaceData;
	delete m_jFaceData;
	delete m_particleData;

	for (const auto& pair : m_particleGroupMap) {
		delete pair.second;
	}
	for (const auto& pair : m_polyDataMap) {
		delete pair.second;
	}

	for (auto r : m_calculatedResults) {
		delete r;
	}
	if (m_pointLocator != nullptr) {
		m_pointLocator->Delete();
	}
}

const std::string& PostZoneDataContainer::zoneName() const
{
	return m_zoneName;
}

SolverDefinitionGridType* PostZoneDataContainer::gridType() const
{
	return m_gridType;
}

vtkPointSetAndValueRangeSet* PostZoneDataContainer::data() const
{
	return m_data;
}

vtkPointSetAndValueRangeSetT<vtkStructuredGrid>* PostZoneDataContainer::edgeIData() const
{
	return m_edgeIData;
}

vtkPointSetAndValueRangeSetT<vtkStructuredGrid>* PostZoneDataContainer::edgeJData() const
{
	return m_edgeJData;
}

vtkPointSetAndValueRangeSetT<vtkStructuredGrid>* PostZoneDataContainer::iFaceData() const
{
	return m_iFaceData;
}

vtkPointSetAndValueRangeSetT<vtkStructuredGrid>* PostZoneDataContainer::jFaceData() const
{
	return m_jFaceData;
}

vtkPointSetAndValueRangeSet* PostZoneDataContainer::data(iRICLib::H5CgnsZone::SolutionPosition position) const
{
	if (position == iRICLib::H5CgnsZone::SolutionPosition::Node || position == iRICLib::H5CgnsZone::SolutionPosition::Cell) {
		return m_data;
	}
	if (position == iRICLib::H5CgnsZone::SolutionPosition::IFace) {
		return m_edgeIData;
	}
	if (position == iRICLib::H5CgnsZone::SolutionPosition::JFace) {
		return m_edgeJData;
	}
	return nullptr;
}

vtkPointSet* PostZoneDataContainer::labelData() const
{
	return m_labelData;
}

vtkPointSetAndValueRangeSetT<vtkPolyData>* PostZoneDataContainer::particleData() const
{
	return m_particleData;
}

const std::map<std::string, vtkPointSetAndValueRangeSetT<vtkPolyData>*>& PostZoneDataContainer::particleGroupMap() const
{
	return m_particleGroupMap;
}

const std::map<std::string, vtkPointSetAndValueRangeSetT<vtkPolyData>*>& PostZoneDataContainer::polyDataMap() const
{
	return m_polyDataMap;
}

vtkPointSetAndValueRangeSetT<vtkPolyData>* PostZoneDataContainer::particleGroup(const std::string& name) const
{
	auto it = m_particleGroupMap.find(name);
	if (it == m_particleGroupMap.end()) {return nullptr;}

	return it->second;
}

vtkPointSetAndValueRangeSetT<vtkPolyData>* PostZoneDataContainer::polyData(const std::string& name) const
{
	auto it = m_polyDataMap.find(name);
	if (it == m_polyDataMap.end()) {return nullptr;}

	return it->second;
}

const std::vector<int>& PostZoneDataContainer::polyDataCellIds(const std::string& name) const
{
	auto it = m_polyDataCellIdsMap.find(name);
	if (it == m_polyDataCellIdsMap.end()) {
		// returns the first.
		it = m_polyDataCellIdsMap.begin();
	}
	return it->second;
}

QString PostZoneDataContainer::caption() const
{
	return zoneName().c_str();
}

bool PostZoneDataContainer::handleCurrentStepUpdate(iRICLib::H5CgnsZone* zone, bool disableCalculatedResult)
{
	loadFromCgnsFile(zone, disableCalculatedResult);
	return m_loadOK;
}

bool PostZoneDataContainer::loadStructuredGrid(iRICLib::H5CgnsZone* zone)
{
	if (m_origData != nullptr) {
		m_origData->Initialize();
		m_edgeIData->data()->Initialize();
		m_edgeJData->data()->Initialize();
		m_iFaceData->data()->Initialize();
		m_jFaceData->data()->Initialize();
		m_labelData->Initialize();
	} else {
		m_origData = vtkSmartPointer<vtkStructuredGrid>::New();
		m_data = new vtkPointSetAndValueRangeSet(m_origData);
		m_origData->Register(nullptr);

		m_edgeIData = new vtkPointSetAndValueRangeSetT<vtkStructuredGrid>();
		m_edgeJData = new vtkPointSetAndValueRangeSetT<vtkStructuredGrid>();
		m_iFaceData = new vtkPointSetAndValueRangeSetT<vtkStructuredGrid>();
		m_jFaceData = new vtkPointSetAndValueRangeSetT<vtkStructuredGrid>();
		m_labelData = vtkSmartPointer<vtkUnstructuredGrid>::New();
	}

	auto grid = vtkStructuredGrid::SafeDownCast(m_origData);
	int NVertexI = 0, NVertexJ = 0, NVertexK = 0;

	int dim = zone->base()->dimension();
	m_size = zone->size();

	if (dim == 1) {
		NVertexI = m_size[0];
		NVertexJ = 1;
		NVertexK = 1;
	} else if (dim == 2) {
		NVertexI = m_size[0];
		NVertexJ = m_size[1];
		NVertexK = 1;
	} else if (dim == 3) {
		NVertexI = m_size[0];
		NVertexJ = m_size[1];
		NVertexK = m_size[2];
	}

	grid->SetDimensions(NVertexI, NVertexJ, NVertexK);

	if (m_pointLocator == nullptr) {
		if (dim < 3) {
			m_pointLocator = vtkStaticPointLocator2D::New();
		} else {
			m_pointLocator = vtkStaticPointLocator::New();
		}
		if (! zone->gridCoordinatesForSolutionExists())
		{
			// m_pointLocator->UseExistingSearchStructureOn(); // vtk 8.1 does not support this
		}
		m_pointLocator->SetDataSet(grid);
	}

	m_edgeIData->concreteData()->SetDimensions(NVertexI, NVertexJ, NVertexK);
	m_edgeJData->concreteData()->SetDimensions(NVertexI, NVertexJ, NVertexK);

	m_iFaceData->concreteData()->SetDimensions(NVertexI, NVertexJ-1, NVertexK);
	m_jFaceData->concreteData()->SetDimensions(NVertexI-1, NVertexJ, NVertexK);

	int numPoints = NVertexI * NVertexJ * NVertexK;
	std::vector<double> dataX(numPoints, 0), dataY(numPoints, 0), dataZ(numPoints, 0);

	iRICLib::H5CgnsGridCoordinates* coords = nullptr;

	if (zone->gridCoordinatesForSolutionExists()) {
		coords = zone->gridCoordinatesForSolution();
	} else {
		coords = zone->gridCoordinates();
	}

	int ier;
	ier = coords->readCoordinatesX(&dataX);
	if (ier != IRIC_NO_ERROR) {return false;}

	if (dim >= 2) {
		ier = coords->readCoordinatesY(&dataY);
		if (ier != IRIC_NO_ERROR) {return false;}
	}

	if (dim == 3) {
		ier = coords->readCoordinatesZ(&dataZ);
		if (ier != IRIC_NO_ERROR) {return false;}
	}

	auto points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();
	auto offset = this->offset();
	for (int k = 0; k < NVertexK; ++k) {
		for (int j = 0; j < NVertexJ; ++j) {
			for (int i = 0; i < NVertexI; ++i) {
				int idx = i + NVertexI * (j + NVertexJ * k);
				points->InsertNextPoint(dataX[idx] - offset.x(), dataY[idx] - offset.y(), dataZ[idx]);
			}
		}
	}

	grid->SetPoints(points);

	//
	//  Vertex(Ni=5, Nj=3) (m_data m_edgeidata m_edgejdata)
	//
	//   V(0,2)------------------V(1,2)------------------V(2,2)------------------V(3,2)------------------V(4,2)
	//      |                       |                       |                       |                       |
	//      |                       |                       |                       |                       |
	//      |                       |                       |                       |                       |
	//      |                       |                       |                       |                       |
	//      |                       |                       |                       |                       |
	//   V(0,1)------------------V(1,1)------------------V(2,1)------------------V(3,1)------------------V(4,1)
	//      |                       |                       |                       |                       |
	//      |                       |                       |                       |                       |
	//      |                       |                       |                       |                       |
	//      |                       |                       |                       |                       |
	//      |                       |                       |                       |                       |
	//   V(0,0)------------------V(1,0)------------------V(2,0)------------------V(3,0)------------------V(4,0)
	//
	//
	//
	//
	//  Vertex(Ni=5, Nj=3)
	//  IFaceCenter(Ni=5 ,Nj=2) (m_ifacedata)
	//
	//
	//   V(0,2)------------------V(1,2)------------------V(2,2)------------------V(3,2)------------------V(4,2)
	//      |                       |                       |                       |                       |
	//      |                       |                       |                       |                       |
	//   I(0,1)                  I(1,1)                  I(2,1)                  I(3,1)                   I(4,1)
	//      |                       |                       |                       |                       |
	//      |                       |                       |                       |                       |
	//   V(0,1)------------------V(1,1)------------------V(2,1)------------------V(3,1)------------------V(4,1)
	//      |                       |                       |                       |                       |
	//      |                       |                       |                       |                       |
	//   I(0,0)                  I(1,0)                  I(2,0)                  I(3,0)                   I(4,0)
	//      |                       |                       |                       |                       |
	//      |                       |                       |                       |                       |
	//   V(0,0)------------------V(1,0)------------------V(2,0)------------------V(3,0)------------------V(4,0)
	//
	//
	//
	//
	//  Vertex(Ni=5, Nj=3)
	//  JFaceCenter(Ni=4, Nj=3) (m_jfacedata)
	//
	//
	//   V(0,2)------J(0,2)------V(1,2)------J(1,2)------V(2,2)------J(2,2)------V(3,2)------J(3,2)------V(4,2)
	//      |                       |                       |                       |                       |
	//      |                       |                       |                       |                       |
	//      |                       |                       |                       |                       |
	//      |                       |                       |                       |                       |
	//      |                       |                       |                       |                       |
	//   V(0,1)------J(0,1)------V(1,1)------J(1,1)------V(2,1)------J(2,1)------V(3,1)------J(3,1)------V(4,1)
	//      |                       |                       |                       |                       |
	//      |                       |                       |                       |                       |
	//      |                       |                       |                       |                       |
	//      |                       |                       |                       |                       |
	//      |                       |                       |                       |                       |
	//   V(0,0)------J(0,0)------V(1,0)------J(1,0)------V(2,0)------J(2,0)------V(3,0)------J(3,0)------V(4,0)
	//
	//

	if (dim >= 2) {
		m_edgeIData->data()->SetPoints(points);
		m_edgeJData->data()->SetPoints(points);

		auto iface_points = vtkSmartPointer<vtkPoints>::New();
		iface_points->SetDataTypeToDouble();
		for (int k = 0; k < NVertexK; ++k) {
			for (int j = 0; j < (NVertexJ - 1); ++j) {
				for (int i = 0; i < NVertexI; ++i) {
					int idx1 = i + NVertexI * (j + NVertexJ * k);
					int idx2 = i + NVertexI * ((j + 1) + NVertexJ * k); // j + 1
					if (dim == 2) {
						iface_points->InsertNextPoint((dataX[idx1] + dataX[idx2]) * 0.5 - offset.x(), (dataY[idx1] + dataY[idx2]) * 0.5 - offset.y(), dataZ[idx1]);
					} else {
						int idx3 = i + NVertexI * (j + NVertexJ * (k + 1)); // k + 1
						iface_points->InsertNextPoint((dataX[idx1] + dataX[idx2]) * 0.5 - offset.x(), (dataY[idx1] + dataY[idx2]) * 0.5 - offset.y(), (dataZ[idx1] + dataZ[idx3]) * 0.5);
					}
				}
			}
		}

		auto jface_points = vtkSmartPointer<vtkPoints>::New();
		jface_points->SetDataTypeToDouble();
		for (int k = 0; k < NVertexK; ++k) {
			for (int j = 0; j < NVertexJ; ++j) {
				for (int i = 0; i < (NVertexI - 1); ++i) {
					int idx1 = i + NVertexI * (j + NVertexJ * k);
					int idx2 = 1 + i + NVertexI * (j + NVertexJ * k); // i + 1
					if (dim == 2) {
						jface_points->InsertNextPoint((dataX[idx1] + dataX[idx2]) * 0.5 - offset.x(), (dataY[idx1] + dataY[idx2]) * 0.5 - offset.y(), dataZ[idx1]);
					}
					else {
						int idx3 = i + NVertexI * (j + NVertexJ * (k + 1)); // k + 1
						jface_points->InsertNextPoint((dataX[idx1] + dataX[idx2]) * 0.5 - offset.x(), (dataY[idx1] + dataY[idx2]) * 0.5 - offset.y(), (dataZ[idx1] + dataZ[idx3]) * 0.5);
					}
				}
			}
		}
		m_iFaceData->data()->SetPoints(iface_points);
		m_jFaceData->data()->SetPoints(jface_points);
	}

	return true;
}

bool PostZoneDataContainer::loadUnstructuredGrid(iRICLib::H5CgnsZone* zone)
{
	if (m_origData != nullptr) {
		vtkUnstructuredGrid::SafeDownCast(m_origData)->Reset();
	} else {
		m_origData = vtkSmartPointer<vtkUnstructuredGrid>::New();
		m_data = new vtkPointSetAndValueRangeSet(m_origData);
		m_origData->Register(nullptr);
		m_labelData = m_origData;
	}

	auto grid = vtkUnstructuredGrid::SafeDownCast(m_origData);

	if (m_pointLocator == nullptr) {
		m_pointLocator = vtkStaticPointLocator2D::New();
		m_pointLocator->SetDataSet(grid);
	}

	int dim = zone->base()->dimension();
	m_size = zone->size();
	auto size = zone->size();
	int NVertex = m_size[0];

	std::vector<double> dataX(NVertex, 0), dataY(NVertex, 0), dataZ(NVertex, 0);

	iRICLib::H5CgnsGridCoordinates* coords = nullptr;

	if (zone->gridCoordinatesForSolutionExists()) {
		coords = zone->gridCoordinatesForSolution();
	} else {
		coords = zone->gridCoordinates();
	}

	int ier;
	ier = coords->readCoordinatesX(&dataX);
	if (ier != IRIC_NO_ERROR) {return false;}

	if (dim >= 2) {
		ier = coords->readCoordinatesY(&dataY);
		if (ier != IRIC_NO_ERROR) {return false;}
	}
	if (dim == 3) {
		ier = coords->readCoordinatesZ(&dataZ);
		if (ier != IRIC_NO_ERROR) {return false;}
	}

	auto points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();
	auto offset = this->offset();
	for (int i = 0; i < NVertex; ++i) {
		points->InsertNextPoint(dataX[i] - offset.x(), dataY[i] - offset.y(), dataZ[i]);
	}
	grid->SetPoints(points);

	// Grid coordinates are loaded.
	// load grid node connectivity data.
	// Unstructured grid that consists of triangles is supported.

	std::vector<int> indices;
	zone->readTriangleElements(&indices);

	int numCells = static_cast<int> (indices.size()) / 3;
	for (int i = 0; i < numCells; ++i) {
		auto tri = vtkSmartPointer<vtkTriangle>::New();
		auto ids = tri->GetPointIds();
		for (int j = 0; j < 3; ++j) {
			ids->SetId(j, indices[i * 3 + j] - 1);
		}
		grid->InsertNextCell(tri->GetCellType(), ids);
	}
	return true;
}

bool PostZoneDataContainer::loadUnstructuredGrid(const int /*fn*/, const int /*currentStep*/)
{
	return true;
}

bool PostZoneDataContainer::loadCellFlagData(iRICLib::H5CgnsZone *zone)
{
	if (zone->base()->dimension() != 2) {
		// cell flag data can be loaded only when m_cellDim == 2.
		return true;
	}

	const auto& conds = m_gridType->gridAttributes();
	std::unordered_set<std::string> valueNames;
	int ier;
	const auto atts = zone->gridAttributes();
	if (atts != nullptr) {
		ier = atts->getValueNames(&valueNames);
	}

	for (auto it = conds.begin(); it != conds.end(); ++it) {
		const SolverDefinitionGridAttribute* cond = *it;
		if (cond->position() != SolverDefinitionGridAttribute::CellCenter) {continue;}
		if (! cond->isOption()) {continue;}

		const SolverDefinitionGridAttributeInteger* icond = dynamic_cast<const SolverDefinitionGridAttributeInteger*>(cond);
		if (icond == 0) {continue;}

		if (valueNames.find(cond->name()) == valueNames.end()) {
			// Corresponding node does not exists.
			// Maybe user is trying to load calculation results in project file created
			// using older solver.
			//
			// initializes the array with default value, and returns true.

			vtkSmartPointer<vtkIntArray> iarray = vtkSmartPointer<vtkIntArray>::New();
			int defaultVal = icond->defaultValue();
			for (vtkIdType i = 0; i < m_origData->GetNumberOfCells(); ++i) {
				iarray->InsertNextValue(defaultVal);
			}
			iarray->SetName(addInputDataPrefix(cond->name()).c_str());

			m_origData->GetCellData()->AddArray(iarray);
			continue;
		}

		std::vector<int> data;
		ier = zone->gridAttributes()->readValue(cond->name(), &data);
		// load data.
		vtkSmartPointer<vtkIntArray> iarray = vtkSmartPointer<vtkIntArray>::New();
		for (int val : data) {
			iarray->InsertNextValue(val);
		}
		iarray->SetName(addInputDataPrefix(cond->name()).c_str());

		m_origData->GetCellData()->AddArray(iarray);
	}
	return true;
}

bool PostZoneDataContainer::setupIndexData()
{
	auto sGrid = vtkStructuredGrid::SafeDownCast(m_origData);
	auto indexArray = vtkSmartPointer<vtkStringArray>::New();
	indexArray->SetName(iRIC::toStr(labelName).c_str());

	if (sGrid != nullptr) {
		std::vector<int> indices;

		// structured grid.
		int dims[3];
		sGrid->GetDimensions(dims);
		QString label;
		if (dims[2] == 1) {
			label = QString("(%1,%2)");
		} else {
			label = QString("(%1,%2,%3)");
		}
		vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
		points->SetDataTypeToDouble();

		double p[3];
		int index;
		for (int i = 0; i < dims[0]; ++i) {
			index = nodeIndex(i, 0, 0);
			sGrid->GetPoint(index, p);
			points->InsertNextPoint(p);
			indexArray->InsertNextValue(iRIC::toStr(label.arg(i + 1).arg(1).arg(1)).c_str());
			indices.push_back(index);
		}
		for (int j = 1; j < dims[1]; ++j) {
			index = nodeIndex(0, j, 0);
			sGrid->GetPoint(index, p);
			points->InsertNextPoint(p);
			indexArray->InsertNextValue(iRIC::toStr(label.arg(1).arg(j + 1).arg(1)).c_str());
			indices.push_back(index);
		}
		for (int k = 1; k < dims[2]; ++k) {
			index = nodeIndex(0, 0, k);
			sGrid->GetPoint(index, p);
			points->InsertNextPoint(p);
			indexArray->InsertNextValue(iRIC::toStr(label.arg(1).arg(1).arg(k + 1)).c_str());
			indices.push_back(index);
		}
		m_labelData->SetPoints(points);
		m_labelData->GetPointData()->AddArray(indexArray);

		auto origPData = m_origData->GetPointData();
		auto labelPData = m_labelData->GetPointData();
		for (const auto& name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(origPData)) {
			auto d1 = origPData->GetArray(name.c_str());
			auto da = vtkDoubleArray::SafeDownCast(d1);
			auto ia = vtkIntArray::SafeDownCast(d1);

			if (da != nullptr) {
				auto d2 = vtkDoubleArray::New();
				d2->SetName(name.c_str());
				for (const auto& index : indices) {
					d2->InsertNextValue(da->GetValue(index));
				}
				labelPData->AddArray(d2);
			} else if (ia != nullptr) {
				auto d2 = vtkIntArray::New();
				d2->SetName(name.c_str());
				for (const auto& index : indices) {
					d2->InsertNextValue(ia->GetValue(index));
				}
				labelPData->AddArray(d2);
			}
		}
	} else {
		// unstructured grid.
		QString label("(%1)");
		for (int i = 0; i < m_origData->GetNumberOfPoints(); ++i) {
			indexArray->InsertNextValue(iRIC::toStr(label.arg(i + 1)).c_str());
		}
		m_labelData->GetPointData()->AddArray(indexArray);
	}
	return true;
}

void PostZoneDataContainer::loadFromCgnsFile(iRICLib::H5CgnsZone* zone)
{
	loadFromCgnsFile(zone, false);
}

void PostZoneDataContainer::loadFromCgnsFile(iRICLib::H5CgnsZone* zone, bool disableCalculatedResult)
{
	m_loadOK = true;

	bool ret;
	if (zone->type() == iRICLib::H5CgnsZone::Type::Structured) {
		ret = loadStructuredGrid(zone);
		if (! ret) {goto ERROR;}
	} else {
		ret = loadUnstructuredGrid(zone);
		if (! ret) {goto ERROR;}
	}

	int ier;
	// node scalar data
	if (zone->nodeSolutionExists() && zone->nodeSolution() != nullptr) {
		ier = CgnsUtil::loadScalarData(zone->nodeSolution(), m_origData->GetPointData(), iRIC::toStr(IBC));
		if (ier != 0) {goto ERROR;}
	}

	// cell scalar data
	if (zone->cellSolutionExists() && zone->cellSolution() != nullptr) {
		ier = CgnsUtil::loadScalarData(zone->cellSolution(), m_origData->GetCellData(), iRIC::toStr(IBC));
		if (ier != 0) {goto ERROR;}
	}

	// edgeI scalar data
	if (zone->iFaceSolutionExists() && zone->iFaceSolution() != nullptr) {
		ier = CgnsUtil::loadEdgeIScalarData(zone->iFaceSolution(), m_edgeIData->data()->GetPointData(), iRIC::toStr(IBC)); // post2d
		if (ier != 0) {goto ERROR;}
		m_edgeIData->updateValueRangeSet();
		ier = CgnsUtil::loadScalarData(zone->iFaceSolution(), m_iFaceData->data()->GetPointData(), iRIC::toStr(IBC)); // for charts
		if (ier != 0) {goto ERROR;}
		m_iFaceData->updateValueRangeSet();
	}

	// edgeJ scalar data
	if (zone->jFaceSolutionExists() && zone->jFaceSolution() != nullptr) {
		ier = CgnsUtil::loadEdgeJScalarData(zone->jFaceSolution(), m_edgeJData->data()->GetPointData(), iRIC::toStr(IBC)); // for post2d
		if (ier != 0) {goto ERROR;}
		m_edgeJData->updateValueRangeSet();
		ier = CgnsUtil::loadScalarData(zone->jFaceSolution(), m_jFaceData->data()->GetPointData(), iRIC::toStr(IBC)); // for charts
		if (ier != 0) {goto ERROR;}
		m_jFaceData->updateValueRangeSet();
	}

	// node vector data
	if (zone->nodeSolutionExists() && zone->nodeSolution() != nullptr) {
		ier = CgnsUtil::loadVectorData(zone->nodeSolution(), m_origData->GetPointData());
		if (ier != 0) {goto ERROR;}
	}

	ret = loadCellFlagData(zone);
	if (ret == false) {goto ERROR;}

	ret = setupIndexData();

	// load particles
	if (zone->particleSolutionExists() && zone->particleSolution() != nullptr) {
		ret = ParticleLoader::load(&m_particleData, zone->particleSolution(), offset());
		if (ret == false) {goto ERROR;}
	} else if (m_particleData != nullptr){
		m_particleData->concreteData()->Initialize();
	}
	// load particleGroup
	if (zone->particleGroupSolutionExists() && zone->particleGroupSolution() != nullptr) {
		ret = ParticleGroupLoader::load(&m_particleGroupMap, zone->particleGroupSolution(), offset());
		if (ret == false) {goto ERROR;}
	} else {
		for (auto& pair : m_particleGroupMap) {
			pair.second->data()->Initialize();
		}
	}

	// load polydata
	if (zone->polyDataSolutionExists() && zone->polyDataSolution() != nullptr) {
		ret = PolyDataLoader::load(&m_polyDataMap, &m_polyDataCellIdsMap, zone->polyDataSolution(), offset());
		if (ret == false) {goto ERROR;}
	} else {
		for (auto& pair : m_polyDataMap) {
			pair.second->data()->Initialize();
		}
	}

	if (! disableCalculatedResult) {
		addCalculatedResultArrays();
	}

	m_data->updateValueRangeSet();

	m_loadedOnce = true;
	emit dataUpdated();
	return;

ERROR:
	m_loadOK = false;
}

void PostZoneDataContainer::loadIfEmpty(iRICLib::H5CgnsZone* zone)
{
	if (m_origData != nullptr) {return;}
	loadFromCgnsFile(zone);
}

int PostZoneDataContainer::index(int dim[3], int i, int j, int k)
{
	Q_ASSERT(0 <= i && i < dim[0]);
	Q_ASSERT(0 <= j && j < dim[1]);
	Q_ASSERT(0 <= k && k < dim[2]);
	return i + dim[0] * (j + dim[1] * k);
}

void PostZoneDataContainer::getIJKIndex(int dim[3], int idx, int* i, int* j, int* k)
{
	Q_ASSERT(0 <= idx && idx <= index(dim, dim[0] - 1, dim[1] - 1, dim[2] - 1));
	*i = idx % (dim[0]);
	*j = ((idx - *i) / dim[0]) % dim[1];
	*k = idx / (dim[0] * dim[1]);
}

int PostZoneDataContainer::nodeIndex(int i, int j, int k) const
{
	int dim[3];
	vtkStructuredGrid* grid = vtkStructuredGrid::SafeDownCast(m_origData);
	grid->GetDimensions(dim);
	return index(dim, i, j, k);
}

void PostZoneDataContainer::getNodeIJKIndex(int index, int* i, int* j, int* k) const
{
	int dim[3];
	vtkStructuredGrid* grid = vtkStructuredGrid::SafeDownCast(m_origData);
	grid->GetDimensions(dim);
	getIJKIndex(dim, index, i, j, k);
}

int PostZoneDataContainer::cellIndex(int i, int j, int k) const
{
	int dim[3];
	vtkStructuredGrid* grid = vtkStructuredGrid::SafeDownCast(m_origData);
	grid->GetCellDims(dim);
	return index(dim, i, j, k);
}

void PostZoneDataContainer::getCellIJKIndex(int index, int* i, int* j, int* k) const
{
	int dim[3];
	vtkStructuredGrid* grid = vtkStructuredGrid::SafeDownCast(m_origData);
	grid->GetCellDims(dim);
	getIJKIndex(dim, index, i, j, k);
}

int PostZoneDataContainer::ifaceIndex(int i, int j, int k) const
{
	int dim[3];
	m_iFaceData->concreteData()->GetDimensions(dim);
	return index(dim, i, j, k);
}

void PostZoneDataContainer::getifaceIJKIndex(int index, int* i, int* j, int* k) const
{
	int dim[3];
	m_iFaceData->concreteData()->GetDimensions(dim);
	getIJKIndex(dim, index, i, j, k);
}

int PostZoneDataContainer::jfaceIndex(int i, int j, int k) const
{
	int dim[3];
	m_jFaceData->concreteData()->GetDimensions(dim);
	return index(dim, i, j, k);
}

void PostZoneDataContainer::getjfaceIJKIndex(int index, int* i, int* j, int* k) const
{
	int dim[3];
	m_jFaceData->concreteData()->GetDimensions(dim);
	getIJKIndex(dim, index, i, j, k);
}

bool PostZoneDataContainer::scalarValueExists() const
{
	auto names = vtkDataSetAttributesTool::getArrayNamesWithOneComponent(m_origData->GetPointData());
	for (const auto& name : names) {
		if (hasInputDataPrefix(name)) {continue;}
		return true;
	}
	return false;
}

bool PostZoneDataContainer::cellScalarValueExists() const
{
	auto names = vtkDataSetAttributesTool::getArrayNamesWithOneComponent(m_origData->GetCellData());
	for (const auto& name : names) {
		if (hasInputDataPrefix(name)) {continue;}
		return true;
	}
	return false;
}

bool PostZoneDataContainer::edgeIScalarValueExists() const
{
	if (m_edgeIData == nullptr) {return false;}
	auto names = vtkDataSetAttributesTool::getArrayNamesWithOneComponent(m_edgeIData->data()->GetPointData());
	return names.size() > 0;
}

bool PostZoneDataContainer::edgeJScalarValueExists() const
{
	if (m_edgeJData == nullptr) {return false;}
	auto names = vtkDataSetAttributesTool::getArrayNamesWithOneComponent(m_edgeJData->data()->GetPointData());
	return names.size() > 0;
}

bool PostZoneDataContainer::vectorValueExists() const
{
	return vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(m_origData->GetPointData()).size() > 0;
}

std::vector<PostCalculatedResult*>& PostZoneDataContainer::calculatedResults()
{
	return m_calculatedResults;
}

const std::vector<PostCalculatedResult*>& PostZoneDataContainer::calculatedResults() const
{
	return m_calculatedResults;
}


bool PostZoneDataContainer::IBCExists(iRICLib::H5CgnsZone::SolutionPosition position) const
{
	if (position == iRICLib::H5CgnsZone::SolutionPosition::Cell) {
		return IBCCellExists();
	} else {
		for (const auto& name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(data(position)->data()->GetPointData())) {
			if (IBC == name.c_str()) {return true;}
		}
	}
	return false;
}

bool PostZoneDataContainer::IBCExists() const
{
	for (const auto& name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(m_origData->GetPointData())) {
		if (IBC == name.c_str()) {return true;}
	}

	return false;
}

bool PostZoneDataContainer::IBCCellExists() const
{
	for (const auto& name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(m_origData->GetCellData())) {
		if (IBC == name.c_str()) {return true;}
	}

	return false;
}

bool PostZoneDataContainer::IBCEdgeIExists() const
{
	for (const auto& name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(m_edgeIData->data()->GetPointData())) {
		if (IBC == name.c_str()) {return true;}
	}

	return false;
}

bool PostZoneDataContainer::IBCEdgeJExists() const
{
	for (const auto& name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(m_edgeJData->data()->GetPointData())) {
		if (IBC == name.c_str()) {return true;}
	}

	return false;
}

QString PostZoneDataContainer::elevationName() const
{
	for (const auto& name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(m_origData->GetPointData())) {
		QString tmpName = name.c_str();
		if (tmpName.toLower().left(9) == "elevation") {
			return tmpName;
		}
	}
	return "";
}

QString PostZoneDataContainer::cellElevationName() const
{
	for (const auto& name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(m_origData->GetCellData())) {
		QString tmpName = name.c_str();
		if (tmpName.toLower().left(9) == "elevation") {
			return tmpName;
		}
	}
	return "";
}

vtkPolyData* PostZoneDataContainer::filteredData(double xmin, double xmax, double ymin, double ymax, bool* masked) const
{
	auto structured = vtkStructuredGrid::SafeDownCast(m_origData);
	if (structured != nullptr) {
		// Structured Data
		return filteredDataStructured(structured, m_pointLocator, xmin, xmax, ymin, ymax, masked);
	} else {
		// Unstructured Data
		return filteredDataUnstructured(m_origData, xmin, xmax, ymin, ymax, masked);
	}
}

vtkPolyData* PostZoneDataContainer::filteredEdgeIData(double xmin, double xmax, double ymin, double ymax, bool* masked) const
{
	if (m_edgeIData == nullptr) {return nullptr;}

	return filteredDataStructured(m_edgeIData->concreteData(), m_pointLocator, xmin, xmax, ymin, ymax, masked);
}

vtkPolyData* PostZoneDataContainer::filteredEdgeJData(double xmin, double xmax, double ymin, double ymax, bool* masked) const
{
	if (m_edgeJData == nullptr) {return nullptr;}

	return filteredDataStructured(m_edgeJData->concreteData(), m_pointLocator, xmin, xmax, ymin, ymax, masked);
}

vtkAbstractPointLocator* PostZoneDataContainer::pointLocator() const
{
	return m_pointLocator;
}

vtkPolyData* PostZoneDataContainer::filteredDataStructured(vtkStructuredGrid* data, vtkAbstractPointLocator* pointLocator, double xmin, double xmax, double ymin, double ymax, bool* masked)
{
	*masked = false;

	bool outOfRegion;
	int iMin, iMax, jMin, jMax;
	int rate;

	bool cullEnable;
	int cullCellLimit, cullIndexLimit;
	Grid::getCullSetting(&cullEnable, &cullCellLimit, &cullIndexLimit);

	RectRegion region(xmin, xmax, ymin, ymax);

	int maxcell = -1;
	if (cullEnable) {
		maxcell = cullCellLimit;
	}
	vtkPointSetRegionAndCellSizeFilter::calcExtractParameters(data, region, pointLocator, maxcell, &outOfRegion, &iMin, &iMax, &jMin, &jMax, &rate);
	auto filteredData = vtkPointSetRegionAndCellSizeFilter::extract(data, iMin, iMax, jMin, jMax, rate);

	auto geo = vtkSmartPointer<vtkGeometryFilter>::New();
	geo->SetInputData(filteredData);
	geo->Update();
	filteredData->Delete();

	auto geoData = geo->GetOutput();
	geoData->Register(nullptr);
	*masked = rate > 1;

	return geoData;
}

vtkPolyData* PostZoneDataContainer::filteredDataUnstructured(vtkPointSet* data, double xmin, double xmax, double ymin, double ymax, bool* masked)
{
	RectRegion region(xmin, xmax, ymin, ymax);

	auto geo = vtkSmartPointer<vtkGeometryFilter>::New();
	geo->SetInputData(data);
	geo->Update();

	return vtkPointSetRegionAndCellSizeFilter::filterGeneral(geo->GetOutput(), region, -1, masked);
}

void PostZoneDataContainer::applyOffset(double x_diff, double y_diff)
{
	doApplyOffset(x_diff, y_diff);
}

void PostZoneDataContainer::doApplyOffset(double x_diff, double y_diff)
{
	doApplyOffset(m_origData, x_diff, y_diff);
	doApplyOffset(m_particleData->data(), x_diff, y_diff);
	for (auto pair : m_polyDataMap) {
		doApplyOffset(pair.second->data(), x_diff, y_diff);
	}
}

void PostZoneDataContainer::doApplyOffset(vtkPointSet* ps, double x_diff, double y_diff)
{
	if (ps == nullptr) {return;}
	vtkPoints* points = ps->GetPoints();
	vtkIdType numPoints = points->GetNumberOfPoints();
	double v[3];
	for (vtkIdType id = 0; id < numPoints; ++id) {
		points->GetPoint(id, v);
		v[0] -= x_diff;
		v[1] -= y_diff;
		points->SetPoint(id, v);
	}
	points->Modified();
}

void PostZoneDataContainer::addCalculatedResultArrays()
{
	for (auto result : m_calculatedResults) {
		if (result->dataType() == PostCalculatedResult::GridNode) {
			auto vals = buildDoubleArray(result->name(), m_origData->GetNumberOfPoints());
			m_origData->GetPointData()->AddArray(vals);
			vals->Delete();
		} else if (result->dataType() == PostCalculatedResult::GridCell) {
			auto vals = buildDoubleArray(result->name(), m_origData->GetNumberOfCells());
			m_origData->GetCellData()->AddArray(vals);
			vals->Delete();
		} else if (result->dataType() == PostCalculatedResult::Particle) {
			auto vals = buildDoubleArray(result->name(), m_origData->GetNumberOfPoints());
			m_origData->GetPointData()->AddArray(vals);
			vals->Delete();
		} else if (result->dataType() == PostCalculatedResult::PolyData) {
			// @todo implement this.
		}
		result->updateValues();
	}
}

void PostZoneDataContainer::doLoadFromProjectMainFile(const QDomNode& node)
{
	for (int i = 0; i < node.childNodes().size(); ++i) {
		auto childNode = node.childNodes().at(i);
		if (childNode.nodeName() == "SimpleOperationResult") {
			auto cr = new PostCalculatedResult(this);
			cr->loadFromProjectMainFile(childNode);
			m_calculatedResults.push_back(cr);
		}
	}
}

void PostZoneDataContainer::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("name", m_zoneName.c_str());

	for (auto result : m_calculatedResults) {
		writer.writeStartElement("SimpleOperationResult");
		result->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

std::vector<PostCalculatedResult*> PostZoneDataContainer::detachCalculatedResult()
{
	std::vector<PostCalculatedResult*> ret;

	for (auto r : m_calculatedResults) {
		r->setParent(nullptr);
		ret.push_back(r);
	}
	m_calculatedResults.clear();

	return ret;
}

void PostZoneDataContainer::attachCalculatedResult(std::vector<PostCalculatedResult*> results)
{
	for (auto r : results) {
		r->setParent(this);
		m_calculatedResults.push_back(r);
	}
}

std::string PostZoneDataContainer::inputDataPrefix()
{
	return "__input__";
}

std::string PostZoneDataContainer::addInputDataPrefix(const std::string& name)
{
	auto newName = inputDataPrefix();
	newName.append(name.c_str());
	return newName;
}

std::string PostZoneDataContainer::removeInputDataPrefix(const std::string& name)
{
	auto prefix = inputDataPrefix();
	return name.substr(prefix.size());
}

bool PostZoneDataContainer::hasInputDataPrefix(const std::string& name)
{
	auto prefix = inputDataPrefix();
	auto subName = name.substr(0, prefix.size());
	return subName == prefix;
}
