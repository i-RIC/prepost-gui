#ifndef POST2DWINDOWGRIDATTRIBUTEJEDGEDATAITEM_H
#define POST2DWINDOWGRIDATTRIBUTEJEDGEDATAITEM_H

#include "post2dwindowgridattributeabstractcelldataitem.h"

class Post2dWindowGridAttributeJEdgeDataItem : public Post2dWindowGridAttributeAbstractCellDataItem
{
	Q_OBJECT

public:
	Post2dWindowGridAttributeJEdgeDataItem(SolverDefinitionGridAttribute* cond, GraphicsWindowDataItem* parent);

private:
	QString positionCaption() const override;
};

#endif // POST2DWINDOWGRIDATTRIBUTEJEDGEDATAITEM_H
