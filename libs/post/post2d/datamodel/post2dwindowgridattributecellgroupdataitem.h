#ifndef POST2DWINDOWGRIDATTRIBUTECELLGROUPDATAITEM_H
#define POST2DWINDOWGRIDATTRIBUTECELLGROUPDATAITEM_H

#include "post2dwindowgridattributeabstractcellgroupdataitem.h"

class Post2dWindowGridAttributeCellGroupDataItem : public Post2dWindowGridAttributeAbstractCellGroupDataItem
{
	Q_OBJECT

public:
	Post2dWindowGridAttributeCellGroupDataItem(Post2dWindowDataItem* parent);

	vtkPointSetExtended* data() const override;

private:
	SolverDefinitionGridAttribute::Position definitionPosition() const override;
	Post2dWindowGridAttributeAbstractCellDataItem* createChild(SolverDefinitionGridAttribute* att) override;
	vtkPointSet* filteredData() const override;
	void getIJIndex(vtkIdType cellId, vtkIdType* i, vtkIdType* j) const override;
};

#endif // POST2DWINDOWGRIDATTRIBUTECELLGROUPDATAITEM_H
