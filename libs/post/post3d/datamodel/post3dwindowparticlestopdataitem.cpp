#include "post3dwindowparticlestopdataitem.h"
#include "post3dwindowzonedataitem.h"

#include <guicore/postcontainer/postzonedatacontainer.h>

Post3dWindowParticlesTopDataItem::Post3dWindowParticlesTopDataItem(Post3dWindowDataItem* p) :
	Post3dWindowParticlesBaseTopDataItem {tr("Particles"), p}
{}

vtkPolyData* Post3dWindowParticlesTopDataItem::particleData() const
{
	return zoneDataItem()->dataContainer()->particleData();
}

Post3dWindowZoneDataItem* Post3dWindowParticlesTopDataItem::zoneDataItem() const
{
	return dynamic_cast<Post3dWindowZoneDataItem*> (parent());
}
