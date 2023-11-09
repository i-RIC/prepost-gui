#ifndef PREPROCESSORGRIDATTRIBUTECELLGROUPDATAITEM_H
#define PREPROCESSORGRIDATTRIBUTECELLGROUPDATAITEM_H

#include "preprocessorgridattributeabstractcellgroupdataitem.h"

class PreProcessorGridAttributeCellGroupDataItem : public PreProcessorGridAttributeAbstractCellGroupDataItem
{
	Q_OBJECT

public:
	PreProcessorGridAttributeCellGroupDataItem(PreProcessorDataItem* parent);

	vtkPointSetExtended* data() const override;

private:
	SolverDefinitionGridAttribute::Position definitionPosition() const override;
	PreProcessorGridAttributeAbstractCellDataItem* createChild(SolverDefinitionGridAttribute* att) override;
	vtkPointSet* filteredData() const override;
	void getIJIndex(vtkIdType cellId, vtkIdType* i, vtkIdType* j) const override;
};

#endif // PREPROCESSORGRIDATTRIBUTECELLGROUPDATAITEM_H
