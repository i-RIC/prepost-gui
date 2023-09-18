#ifndef POST2DWINDOWNODESCALARGROUPTOPDATAITEM_ATTRIBUTEBROWSERCONTROLLER_H
#define POST2DWINDOWNODESCALARGROUPTOPDATAITEM_ATTRIBUTEBROWSERCONTROLLER_H

#include "../../post2dwindowattributebrowsercontroller.h"
#include "../post2dwindownodescalargrouptopdataitem.h"

class v4PostZoneDataContainer;

class Post2dWindowNodeScalarGroupTopDataItem::AttributeBrowserController : public Post2dWindowAttributeBrowserController
{
public:
	AttributeBrowserController(Post2dWindowNodeScalarGroupTopDataItem* item);

	void initialize() override;

private:
	bool doUpdate(const QPoint& p, VTKGraphicsView* v) override;

	v4PostZoneDataContainer* dataContainer() const;
};

#endif // POST2DWINDOWNODESCALARGROUPTOPDATAITEM_ATTRIBUTEBROWSERCONTROLLER_H
