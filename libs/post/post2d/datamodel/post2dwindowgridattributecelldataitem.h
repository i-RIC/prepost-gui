#ifndef POST2DWINDOWGRIDATTRIBUTECELLDATAITEM_H
#define POST2DWINDOWGRIDATTRIBUTECELLDATAITEM_H

#include "post2dwindowgridattributeabstractcelldataitem.h"

class Post2dWindowGridAttributeCellDataItem : public Post2dWindowGridAttributeAbstractCellDataItem
{
	Q_OBJECT

public:
	Post2dWindowGridAttributeCellDataItem(SolverDefinitionGridAttribute* cond, GraphicsWindowDataItem* parent);
	QDialog* propertyDialog(QWidget* parent) override;

private:
	QString positionCaption() const override;
};

#endif // POST2DWINDOWGRIDATTRIBUTECELLDATAITEM_H
