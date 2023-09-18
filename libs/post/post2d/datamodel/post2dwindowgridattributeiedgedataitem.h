#ifndef POST2DWINDOWGRIDATTRIBUTEIEDGEDATAITEM_H
#define POST2DWINDOWGRIDATTRIBUTEIEDGEDATAITEM_H

#include "post2dwindowgridattributeabstractcelldataitem.h"

class Post2dWindowGridAttributeIEdgeDataItem : public Post2dWindowGridAttributeAbstractCellDataItem
{
	Q_OBJECT

public:
	Post2dWindowGridAttributeIEdgeDataItem(SolverDefinitionGridAttribute* cond, GraphicsWindowDataItem* parent);

private:
	QString positionCaption() const override;
};

#endif // POST2DWINDOWGRIDATTRIBUTEIEDGEDATAITEM_H
