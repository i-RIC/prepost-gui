#include "post2dwindowinputgriddataitem.h"
#include "post2dwindowgridattributejedgedataitem.h"
#include "post2dwindowgridattributejedgegroupdataitem.h"

#include <guibase/vtkpointsetextended/vtkpolydataextended2d.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/pre/grid/v4inputgrid.h>

Post2dWindowGridAttributeJEdgeGroupDataItem::Post2dWindowGridAttributeJEdgeGroupDataItem(Post2dWindowDataItem* parent) :
	Post2dWindowGridAttributeAbstractCellGroupDataItem(tr("Edge (J-direction) attributes"), parent)
{}

vtkPointSetExtended* Post2dWindowGridAttributeJEdgeGroupDataItem::data() const
{
	auto sGrid = dynamic_cast<v4Structured2dGrid*> (gridDataItem()->grid());
	return sGrid->vtkJEdgeData();
}

SolverDefinitionGridAttribute::Position Post2dWindowGridAttributeJEdgeGroupDataItem::definitionPosition() const
{
	return SolverDefinitionGridAttribute::Position::JFace;
}

Post2dWindowGridAttributeAbstractCellDataItem* Post2dWindowGridAttributeJEdgeGroupDataItem::createChild(SolverDefinitionGridAttribute* att)
{
	return new Post2dWindowGridAttributeJEdgeDataItem(att, this);
}

vtkPointSet* Post2dWindowGridAttributeJEdgeGroupDataItem::filteredData() const
{
	auto sGrid = dynamic_cast<v4Structured2dGrid*> (gridDataItem()->grid());
	return sGrid->vtkJEdgeFilteredData();
}

void Post2dWindowGridAttributeJEdgeGroupDataItem::getIJIndex(vtkIdType cellId, vtkIdType* i, vtkIdType* j) const
{
	auto sGrid = dynamic_cast<v4Structured2dGrid*> (gridDataItem()->grid());
	sGrid->getJEdgeIJIndex(cellId, i, j);
}
