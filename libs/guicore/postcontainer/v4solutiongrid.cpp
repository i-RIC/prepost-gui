#include "../grid/v4structured15dgridwithcrosssection.h"
#include "../grid/v4structured2dgrid.h"
#include "../grid/v4structured3dgrid.h"
#include "../grid/v4unstructured2dgrid.h"

#include "v4solutiongrid.h"
#include "private/v4solutiongrid_attributedataprovider.h"
#include "private/v4solutiongrid_impl.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guibase/vtkpointsetextended/vtkpolydataextended2d.h>
#include <guibase/vtkpointsetextended/vtkpolydataextended3d.h>

v4SolutionGrid::Impl::Impl(SolverDefinitionGridType* gridType, v4Grid* grid) :
	m_gridType {gridType},
	m_grid {grid}
{}

vtkDataSetAttributes* v4SolutionGrid::Impl::attributes(Position pos)
{
	if (pos == Position::Node) {
		return m_grid->vtkData()->data()->GetPointData();
	} else if (pos == Position::CellCenter) {
		return m_grid->vtkData()->data()->GetCellData();
	} else if (pos == Position::IFace) {
		auto grid2d = dynamic_cast<v4Structured2dGrid*> (m_grid);
		if (grid2d != nullptr) {return grid2d->vtkIEdgeData()->data()->GetCellData();}
		auto grid3d = dynamic_cast<v4Structured3dGrid*> (m_grid);
		if (grid3d != nullptr) {return grid3d->vtkIFaceData()->data()->GetCellData();}
	} else if (pos == Position::JFace) {
		auto grid2d = dynamic_cast<v4Structured2dGrid*> (m_grid);
		if (grid2d != nullptr) {return grid2d->vtkJEdgeData()->data()->GetCellData();}
		auto grid3d = dynamic_cast<v4Structured3dGrid*> (m_grid);
		if (grid3d != nullptr) {return grid3d->vtkJFaceData()->data()->GetCellData();}
	} else if (pos == Position::KFace) {
		auto grid3d = dynamic_cast<v4Structured3dGrid*> (m_grid);
		if (grid3d != nullptr) {return grid3d->vtkKFaceData()->data()->GetCellData();}
	}

	return nullptr;
}

const std::string v4SolutionGrid::IBC {"IBC"};
const double v4SolutionGrid::IBCLimit {0.99};

v4SolutionGrid::v4SolutionGrid(SolverDefinitionGridType* gridType, v4Grid* grid) :
	impl {new Impl {gridType, grid}}
{
	auto provider = new AttributeDataProvider(gridType);
	grid->setAttributeDataProvider(provider);
}

v4SolutionGrid::~v4SolutionGrid()
{}

SolverDefinitionGridType* v4SolutionGrid::gridType() const
{
	return impl->m_gridType;
}

SolverDefinitionGridType::GridType v4SolutionGrid::type() const
{
	auto g = impl->m_grid;
	if (dynamic_cast<v4Structured15dGridWithCrossSection*> (g) != nullptr) {
		return SolverDefinitionGridType::GridType::gtNormal1_5DGridWithCrosssection;
	} else if (dynamic_cast<v4Structured2dGrid*> (g) != nullptr) {
		return SolverDefinitionGridType::GridType::gtStructured2DGrid;
	} else if (dynamic_cast<v4Unstructured2dGrid*> (g) != nullptr) {
		return SolverDefinitionGridType::GridType::gtUnstructured2DGrid;
	}

	return SolverDefinitionGridType::GridType::gtUnknownGrid;
}

v4Grid* v4SolutionGrid::grid() const
{
	return impl->m_grid;
}

bool v4SolutionGrid::scalarValueExists(Position pos) const
{
	auto data = impl->attributes(pos);
	if (data == nullptr) {return false;}

	auto names = vtkDataSetAttributesTool::getArrayNamesWithOneComponent(data);
	return names.size() > 0;
}

bool v4SolutionGrid::vectorValueExists(Position pos) const
{
	auto data = impl->attributes(pos);
	if (data == nullptr) {return false;}

	auto names = vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(data);
	return names.size() > 0;
}

bool v4SolutionGrid::ibcExists(Position pos) const
{
	auto data = impl->attributes(pos);
	if (data == nullptr) {return false;}

	auto names = vtkDataSetAttributesTool::getArrayNamesWithOneComponent(data);
	for (const auto& name : names) {
		if (name == IBC) {return true;}
	}
	return false;
}

std::string v4SolutionGrid::elevationName(Position pos) const
{
	auto data = impl->attributes(pos);
	if (data == nullptr) {return "";}

	auto names = vtkDataSetAttributesTool::getArrayNamesWithOneComponent(data);
	for (const auto& name : names) {
		if (name.substr(0, 9) == "elevation") {
			return name;
		}
	}
	return "";
}
