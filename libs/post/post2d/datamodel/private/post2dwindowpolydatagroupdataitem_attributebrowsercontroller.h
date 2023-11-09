#ifndef POST2DWINDOWPOLYDATAGROUPDATAITEM_ATTRIBUTEBROWSERCONTROLLER_H
#define POST2DWINDOWPOLYDATAGROUPDATAITEM_ATTRIBUTEBROWSERCONTROLLER_H

#include "../../post2dwindowattributebrowsercontroller.h"
#include "../post2dwindowpolydatagroupdataitem.h"

class Post2dWindowPolyDataGroupDataItem::AttributeBrowserController : public Post2dWindowAttributeBrowserController
{
public:
	AttributeBrowserController(Post2dWindowPolyDataGroupDataItem* item);

	void initialize() override;

private:
	bool doUpdate(const QPoint& p, VTKGraphicsView* v) override;
};

#endif // POST2DWINDOWPOLYDATAGROUPDATAITEM_ATTRIBUTEBROWSERCONTROLLER_H
