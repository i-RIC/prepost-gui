#ifndef POST2DWINDOWCELLSCALARGROUPTOPDATAITEM_H
#define POST2DWINDOWCELLSCALARGROUPTOPDATAITEM_H

#include "post2dwindowabstractcellscalargrouptopdataitem.h"

class Post2dWindowCellScalarGroupTopDataItem : public Post2dWindowAbstractCellScalarGroupTopDataItem
{
	Q_OBJECT

public:
	Post2dWindowCellScalarGroupTopDataItem(Post2dWindowDataItem* parent);

private:
	void getIJIndex(vtkIdType idx, vtkIdType* i, vtkIdType* j) const override;
	vtkPointSetExtended* data() const override;
	Post2dWindowAbstractCellScalarGroupDataItem* createChild(const std::string& target) override;
};

#endif // POST2DWINDOWCELLSCALARGROUPTOPDATAITEM_H
