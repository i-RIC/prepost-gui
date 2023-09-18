#ifndef POST2DWINDOWABSTRACTCELLSCALARGROUPTOPDATAITEM_ATTRIBUTEBROWSERCONTROLLER_H
#define POST2DWINDOWABSTRACTCELLSCALARGROUPTOPDATAITEM_ATTRIBUTEBROWSERCONTROLLER_H

#include "../../post2dwindowattributebrowsercontroller.h"
#include "../post2dwindowabstractcellscalargrouptopdataitem.h"

class v4PostZoneDataContainer;

class Post2dWindowAbstractCellScalarGroupTopDataItem::AttributeBrowserController : public Post2dWindowAttributeBrowserController
{
public:
	AttributeBrowserController(Post2dWindowAbstractCellScalarGroupTopDataItem* item);

	void initialize() override;

private:
	bool doUpdate(const QPoint& p, VTKGraphicsView* v) override;

	v4PostZoneDataContainer* dataContainer() const;
};

#endif // POST2DWINDOWABSTRACTCELLSCALARGROUPTOPDATAITEM_ATTRIBUTEBROWSERCONTROLLER_H
