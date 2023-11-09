#include "preprocessorgriddataitem.h"
#include "preprocessorgridattributeiedgedataitem.h"
#include "preprocessorgridattributeiedgegroupdataitem.h"

#include <guibase/vtkpointsetextended/vtkpolydataextended2d.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/pre/grid/v4inputgrid.h>

PreProcessorGridAttributeIEdgeGroupDataItem::PreProcessorGridAttributeIEdgeGroupDataItem(PreProcessorDataItem* parent) :
	PreProcessorGridAttributeAbstractCellGroupDataItem(tr("Edge (I-direction) attributes"), parent)
{}

vtkPointSetExtended* PreProcessorGridAttributeIEdgeGroupDataItem::data() const
{
	auto sGrid = dynamic_cast<v4Structured2dGrid*> (gridDataItem()->grid()->grid());
	return sGrid->vtkIEdgeData();
}

SolverDefinitionGridAttribute::Position PreProcessorGridAttributeIEdgeGroupDataItem::definitionPosition() const
{
	return SolverDefinitionGridAttribute::Position::IFace;
}

PreProcessorGridAttributeAbstractCellDataItem* PreProcessorGridAttributeIEdgeGroupDataItem::createChild(SolverDefinitionGridAttribute* att)
{
	return new PreProcessorGridAttributeIEdgeDataItem(att, this);
}

vtkPointSet* PreProcessorGridAttributeIEdgeGroupDataItem::filteredData() const
{
	auto sGrid = dynamic_cast<v4Structured2dGrid*> (gridDataItem()->grid()->grid());
	return sGrid->vtkIEdgeFilteredData();
}

void PreProcessorGridAttributeIEdgeGroupDataItem::getIJIndex(vtkIdType cellId, vtkIdType* i, vtkIdType* j) const
{
	auto sGrid = dynamic_cast<v4Structured2dGrid*> (gridDataItem()->grid()->grid());
	sGrid->getIEdgeIJIndex(cellId, i, j);
}
