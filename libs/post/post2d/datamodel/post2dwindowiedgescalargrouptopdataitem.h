#ifndef POST2DWINDOWIEDGESCALARGROUPTOPDATAITEM_H
#define POST2DWINDOWIEDGESCALARGROUPTOPDATAITEM_H

#include "post2dwindowabstractcellscalargrouptopdataitem.h"

class Post2dWindowIEdgeScalarGroupTopDataItem : public Post2dWindowAbstractCellScalarGroupTopDataItem
{
	Q_OBJECT

public:
	Post2dWindowIEdgeScalarGroupTopDataItem(Post2dWindowDataItem* parent);

private:
	void getIJIndex(vtkIdType idx, vtkIdType* i, vtkIdType* j) const override;
	vtkPointSetExtended* data() const override;
	Post2dWindowAbstractCellScalarGroupDataItem* createChild(const std::string& target) override;
};

#endif // POST2DWINDOWIEDGESCALARGROUPTOPDATAITEM_H
