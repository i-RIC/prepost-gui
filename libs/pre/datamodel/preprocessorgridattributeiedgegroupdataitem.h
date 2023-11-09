#ifndef PREPROCESSORGRIDATTRIBUTEIEDGEGROUPDATAITEM_H
#define PREPROCESSORGRIDATTRIBUTEIEDGEGROUPDATAITEM_H

#include "preprocessorgridattributeabstractcellgroupdataitem.h"

class PreProcessorGridAttributeIEdgeGroupDataItem : public PreProcessorGridAttributeAbstractCellGroupDataItem
{
public:
	PreProcessorGridAttributeIEdgeGroupDataItem(PreProcessorDataItem* parent);

	vtkPointSetExtended* data() const override;

private:
	SolverDefinitionGridAttribute::Position definitionPosition() const override;
	PreProcessorGridAttributeAbstractCellDataItem* createChild(SolverDefinitionGridAttribute* att) override;
	vtkPointSet* filteredData() const override;
	void getIJIndex(vtkIdType cellId, vtkIdType* i, vtkIdType* j) const override;
};

#endif // PREPROCESSORGRIDATTRIBUTEIEDGEGROUPDATAITEM_H
