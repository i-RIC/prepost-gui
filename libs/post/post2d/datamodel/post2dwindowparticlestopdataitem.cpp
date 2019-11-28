#include "post2dwindowparticlestopdataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guicore/postcontainer/postzonedatacontainer.h>

Post2dWindowParticlesTopDataItem::Post2dWindowParticlesTopDataItem(Post2dWindowDataItem* p) :
	Post2dWindowParticlesBaseTopDataItem {tr("Particles"), p}
{}

vtkPolyData* Post2dWindowParticlesTopDataItem::particleData() const
{
	return zoneDataItem()->dataContainer()->particleData();
}

Post2dWindowZoneDataItem* Post2dWindowParticlesTopDataItem::zoneDataItem() const
{
	return dynamic_cast<Post2dWindowZoneDataItem*> (parent());
}
