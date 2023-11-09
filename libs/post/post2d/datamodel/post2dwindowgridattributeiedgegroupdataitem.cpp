#include "post2dwindowinputgriddataitem.h"
#include "post2dwindowgridattributeiedgedataitem.h"
#include "post2dwindowgridattributeiedgegroupdataitem.h"

#include <guibase/vtkpointsetextended/vtkpolydataextended2d.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/pre/grid/v4inputgrid.h>

Post2dWindowGridAttributeIEdgeGroupDataItem::Post2dWindowGridAttributeIEdgeGroupDataItem(Post2dWindowDataItem* parent) :
	Post2dWindowGridAttributeAbstractCellGroupDataItem(tr("Edge (I-direction) attributes"), parent)
{}

vtkPointSetExtended* Post2dWindowGridAttributeIEdgeGroupDataItem::data() const
{
	auto sGrid = dynamic_cast<v4Structured2dGrid*> (gridDataItem()->grid());
	return sGrid->vtkIEdgeData();
}

SolverDefinitionGridAttribute::Position Post2dWindowGridAttributeIEdgeGroupDataItem::definitionPosition() const
{
	return SolverDefinitionGridAttribute::Position::IFace;
}

Post2dWindowGridAttributeAbstractCellDataItem* Post2dWindowGridAttributeIEdgeGroupDataItem::createChild(SolverDefinitionGridAttribute* att)
{
	return new Post2dWindowGridAttributeIEdgeDataItem(att, this);
}

vtkPointSet* Post2dWindowGridAttributeIEdgeGroupDataItem::filteredData() const
{
	auto sGrid = dynamic_cast<v4Structured2dGrid*> (gridDataItem()->grid());
	return sGrid->vtkIEdgeFilteredData();
}

void Post2dWindowGridAttributeIEdgeGroupDataItem::getIJIndex(vtkIdType cellId, vtkIdType* i, vtkIdType* j) const
{
	auto sGrid = dynamic_cast<v4Structured2dGrid*> (gridDataItem()->grid());
	sGrid->getIEdgeIJIndex(cellId, i, j);
}
