#ifndef PREPROCESSORGRIDATTRIBUTEJEDGEGROUPDATAITEM_H
#define PREPROCESSORGRIDATTRIBUTEJEDGEGROUPDATAITEM_H

#include "preprocessorgridattributeabstractcellgroupdataitem.h"

class PreProcessorGridAttributeJEdgeGroupDataItem : public PreProcessorGridAttributeAbstractCellGroupDataItem
{
public:
	PreProcessorGridAttributeJEdgeGroupDataItem(PreProcessorDataItem* parent);

	vtkPointSetExtended* data() const override;

private:
	SolverDefinitionGridAttribute::Position definitionPosition() const override;
	PreProcessorGridAttributeAbstractCellDataItem* createChild(SolverDefinitionGridAttribute* att) override;
	vtkPointSet* filteredData() const override;
	void getIJIndex(vtkIdType cellId, vtkIdType* i, vtkIdType* j) const override;
};

#endif // PREPROCESSORGRIDATTRIBUTEJEDGEGROUPDATAITEM_H
