#ifndef POST2DWINDOWJEDGESCALARGROUPTOPDATAITEM_H
#define POST2DWINDOWJEDGESCALARGROUPTOPDATAITEM_H

#include "post2dwindowabstractcellscalargrouptopdataitem.h"

class Post2dWindowJEdgeScalarGroupTopDataItem : public Post2dWindowAbstractCellScalarGroupTopDataItem
{
	Q_OBJECT

public:
	Post2dWindowJEdgeScalarGroupTopDataItem(Post2dWindowDataItem* parent);

private:
	void getIJIndex(vtkIdType idx, vtkIdType* i, vtkIdType* j) const override;
	vtkPointSetExtended* data() const override;
	Post2dWindowAbstractCellScalarGroupDataItem* createChild(const std::string& target) override;
};

#endif // POST2DWINDOWJEDGESCALARGROUPTOPDATAITEM_H
