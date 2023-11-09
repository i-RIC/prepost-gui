#include "preprocessorgriddataitem.h"
#include "preprocessorgridattributecelldataitem.h"
#include "preprocessorgridattributecellgroupdataitem.h"

#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/pre/grid/v4inputgrid.h>

PreProcessorGridAttributeCellGroupDataItem::PreProcessorGridAttributeCellGroupDataItem(PreProcessorDataItem* parent) :
	PreProcessorGridAttributeAbstractCellGroupDataItem(tr("Cell attributes"), parent)
{}

vtkPointSetExtended* PreProcessorGridAttributeCellGroupDataItem::data() const
{
	return gridDataItem()->grid()->grid()->vtkData();
}

SolverDefinitionGridAttribute::Position PreProcessorGridAttributeCellGroupDataItem::definitionPosition() const
{
	return SolverDefinitionGridAttribute::Position::CellCenter;
}

PreProcessorGridAttributeAbstractCellDataItem* PreProcessorGridAttributeCellGroupDataItem::createChild(SolverDefinitionGridAttribute* att)
{
	return new PreProcessorGridAttributeCellDataItem(att, this);
}

vtkPointSet* PreProcessorGridAttributeCellGroupDataItem::filteredData() const
{
	auto grid2d = dynamic_cast<v4Grid2d*> (gridDataItem()->grid()->grid());
	return grid2d->vtkFilteredData();
}

void PreProcessorGridAttributeCellGroupDataItem::getIJIndex(vtkIdType cellId, vtkIdType* i, vtkIdType* j) const
{
	auto sgrid = dynamic_cast<v4Structured2dGrid*> (gridDataItem()->grid()->grid());
	sgrid->getCellIJIndex(cellId, i, j);
}
