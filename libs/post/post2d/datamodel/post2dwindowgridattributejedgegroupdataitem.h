#ifndef POST2DWINDOWGRIDATTRIBUTEJEDGEGROUPDATAITEM_H
#define POST2DWINDOWGRIDATTRIBUTEJEDGEGROUPDATAITEM_H

#include "post2dwindowgridattributeabstractcellgroupdataitem.h"

class Post2dWindowGridAttributeJEdgeGroupDataItem : public Post2dWindowGridAttributeAbstractCellGroupDataItem
{
public:
	Post2dWindowGridAttributeJEdgeGroupDataItem(Post2dWindowDataItem* parent);

	vtkPointSetExtended* data() const override;

private:
	SolverDefinitionGridAttribute::Position definitionPosition() const override;
	Post2dWindowGridAttributeAbstractCellDataItem* createChild(SolverDefinitionGridAttribute* att) override;
	vtkPointSet* filteredData() const override;
	void getIJIndex(vtkIdType cellId, vtkIdType* i, vtkIdType* j) const override;
};

#endif // POST2DWINDOWGRIDATTRIBUTEJEDGEGROUPDATAITEM_H
