#include "post2dwindowinputgriddataitem.h"
#include "post2dwindowgridattributecelldataitem.h"
#include "post2dwindowgridattributecellgroupdataitem.h"

#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/pre/grid/v4inputgrid.h>

Post2dWindowGridAttributeCellGroupDataItem::Post2dWindowGridAttributeCellGroupDataItem(Post2dWindowDataItem* parent) :
	Post2dWindowGridAttributeAbstractCellGroupDataItem(tr("Cell attributes"), parent)
{}

vtkPointSetExtended* Post2dWindowGridAttributeCellGroupDataItem::data() const
{
	return gridDataItem()->grid()->vtkData();
}

SolverDefinitionGridAttribute::Position Post2dWindowGridAttributeCellGroupDataItem::definitionPosition() const
{
	return SolverDefinitionGridAttribute::Position::CellCenter;
}

Post2dWindowGridAttributeAbstractCellDataItem* Post2dWindowGridAttributeCellGroupDataItem::createChild(SolverDefinitionGridAttribute* att)
{
	return new Post2dWindowGridAttributeCellDataItem(att, this);
}

vtkPointSet* Post2dWindowGridAttributeCellGroupDataItem::filteredData() const
{
	auto grid2d = dynamic_cast<v4Grid2d*> (gridDataItem()->grid());
	return grid2d->vtkFilteredData();
}

void Post2dWindowGridAttributeCellGroupDataItem::getIJIndex(vtkIdType cellId, vtkIdType* i, vtkIdType* j) const
{
	auto sgrid = dynamic_cast<v4Structured2dGrid*> (gridDataItem()->grid());
	sgrid->getCellIJIndex(cellId, i, j);
}
