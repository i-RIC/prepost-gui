#ifndef V4INPUTGRID_H
#define V4INPUTGRID_H

#include "../../guicore_global.h"

#include "../../solverdef/solverdefinitiongridtype.h"

#include <vector>

class GridAttributeContainer;
class PreProcessorGridDataItemI;
class v4Grid;

class GUICOREDLL_EXPORT v4InputGrid
{
public:
	v4InputGrid(SolverDefinitionGridType* gridType, v4Grid* grid);
	~v4InputGrid();

	SolverDefinitionGridType* gridType() const;
	SolverDefinitionGridType::GridType type() const;

	v4Grid* grid() const;

	bool isModified();
	void setIsModified(bool isModified);

	void addAttribute(GridAttributeContainer* att);
	const std::vector<GridAttributeContainer*>& attributes() const;
	GridAttributeContainer* attribute(const std::string& name) const;
	bool hasGeneratingAttributes() const;

	void allocateAttributes();

	PreProcessorGridDataItemI* gridDataItem() const;
	void setGridDataItem(PreProcessorGridDataItemI* gridDataItem);

private:
	class Impl;
	std::unique_ptr<Impl> impl;

	class AttributeDataProvider;
};

#endif // V4INPUTGRID_H
