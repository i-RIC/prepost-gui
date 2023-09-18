#ifndef V4INPUTGRID_IMPL_H
#define V4INPUTGRID_IMPL_H

#include "../v4inputgrid.h"

#include <unordered_map>

class v4InputGrid::Impl
{
public:
	Impl(SolverDefinitionGridType* gridType, v4Grid* grid);
	~Impl();

	SolverDefinitionGridType* m_gridType;
	v4Grid* m_grid;

	bool m_isModified;

	std::vector<GridAttributeContainer*> m_attributes;
	std::unordered_map<std::string, GridAttributeContainer*> m_attributeNameMap;

	PreProcessorGridDataItemI* m_gridDataItem;
};

#endif // V4INPUTGRID_IMPL_H
