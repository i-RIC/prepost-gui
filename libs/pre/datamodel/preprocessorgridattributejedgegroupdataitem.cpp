#include "preprocessorgriddataitem.h"
#include "preprocessorgridattributejedgedataitem.h"
#include "preprocessorgridattributejedgegroupdataitem.h"

#include <guibase/vtkpointsetextended/vtkpolydataextended2d.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/pre/grid/v4inputgrid.h>

PreProcessorGridAttributeJEdgeGroupDataItem::PreProcessorGridAttributeJEdgeGroupDataItem(PreProcessorDataItem* parent) :
	PreProcessorGridAttributeAbstractCellGroupDataItem(tr("Edge (J-direction) attributes"), parent)
{}

vtkPointSetExtended* PreProcessorGridAttributeJEdgeGroupDataItem::data() const
{
	auto sGrid = dynamic_cast<v4Structured2dGrid*> (gridDataItem()->grid()->grid());
	return sGrid->vtkJEdgeData();
}

SolverDefinitionGridAttribute::Position PreProcessorGridAttributeJEdgeGroupDataItem::definitionPosition() const
{
	return SolverDefinitionGridAttribute::Position::JFace;
}

PreProcessorGridAttributeAbstractCellDataItem* PreProcessorGridAttributeJEdgeGroupDataItem::createChild(SolverDefinitionGridAttribute* att)
{
	return new PreProcessorGridAttributeJEdgeDataItem(att, this);
}

vtkPointSet* PreProcessorGridAttributeJEdgeGroupDataItem::filteredData() const
{
	auto sGrid = dynamic_cast<v4Structured2dGrid*> (gridDataItem()->grid()->grid());
	return sGrid->vtkJEdgeFilteredData();
}

void PreProcessorGridAttributeJEdgeGroupDataItem::getIJIndex(vtkIdType cellId, vtkIdType* i, vtkIdType* j) const
{
	auto sGrid = dynamic_cast<v4Structured2dGrid*> (gridDataItem()->grid()->grid());
	sGrid->getJEdgeIJIndex(cellId, i, j);
}
