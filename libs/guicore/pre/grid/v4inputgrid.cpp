#include "../../solverdef/solverdefinitiongridattribute.h"
#include "../../solverdef/solverdefinitiongridtype.h"
#include "../base/preprocessorgriddataitemi.h"
#include "../gridcond/base/gridattributecontainer.h"
#include "../../project/projectdata.h"

#include "../../grid/v4structured15dgridwithcrosssection.h"
#include "../../grid/v4structured2dgrid.h"
#include "../../grid/v4unstructured2dgrid.h"

#include "v4inputgrid.h"
#include "private/v4inputgrid_attributedataprovider.h"
#include "private/v4inputgrid_impl.h"

v4InputGrid::Impl::Impl(SolverDefinitionGridType* gridType, v4Grid* grid) :
	m_gridType {gridType},
	m_grid {grid},
	m_isModified {false},
	m_attributes {},
	m_attributeNameMap {},
	m_gridDataItem {nullptr}
{}

v4InputGrid::Impl::~Impl()
{
	for (auto att : m_attributes) {
		delete att;
	}
}

v4InputGrid::v4InputGrid(SolverDefinitionGridType* gridType, v4Grid* grid) :
	impl {new Impl {gridType, grid}}
{
	auto provider = new AttributeDataProvider(gridType);
	grid->setAttributeDataProvider(provider);
}

v4InputGrid::~v4InputGrid()
{}

SolverDefinitionGridType* v4InputGrid::gridType() const
{
	return impl->m_gridType;
}

SolverDefinitionGridType::GridType v4InputGrid::type() const
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

v4Grid* v4InputGrid::grid() const
{
	return impl->m_grid;
}

bool v4InputGrid::isModified()
{
	return impl->m_isModified;
}

void v4InputGrid::setIsModified(bool isModified)
{
	impl->m_isModified = isModified;
}

void v4InputGrid::addAttribute(GridAttributeContainer* att)
{
	impl->m_attributes.push_back(att);
	impl->m_attributeNameMap.insert({att->name(), att});
}

const std::vector<GridAttributeContainer*>& v4InputGrid::attributes() const
{
	return impl->m_attributes;
}

GridAttributeContainer* v4InputGrid::attribute(const std::string& name) const
{
	auto it = impl->m_attributeNameMap.find(name);
	if (it == impl->m_attributeNameMap.end()) {return nullptr;}

	return it->second;
}

bool v4InputGrid::hasGeneratingAttributes() const
{
	for (auto att : impl->m_attributes) {
		auto def = att->gridAttribute();
		if (! def->mapping().isEmpty()) {return true;}
	}

	return false;
}

void v4InputGrid::allocateAttributes()
{
	for (auto att : attributes()) {
		att->allocate();
	}
}

PreProcessorGridDataItemI* v4InputGrid::gridDataItem() const
{
	return impl->m_gridDataItem;
}

void v4InputGrid::setGridDataItem(PreProcessorGridDataItemI* gridDataItem)
{
	impl->m_gridDataItem = gridDataItem;
	for (auto att : attributes()) {
		att->setTemporaryDir(gridDataItem->subPath());
	}
}
