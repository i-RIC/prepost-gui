#include "post3dwindowparticlestopdataitem.h"
#include "post3dwindowzonedataitem.h"

#include <guicore/grid/v4particles3d.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>

Post3dWindowParticlesTopDataItem::Post3dWindowParticlesTopDataItem(Post3dWindowDataItem* p) :
	Post3dWindowParticlesBaseTopDataItem {tr("Particles"), p}
{}

v4Particles3d* Post3dWindowParticlesTopDataItem::particleData() const
{
	auto cont = zoneDataItem()->v4DataContainer();
	if (cont == nullptr) {return nullptr;}

	return dynamic_cast<v4Particles3d*> (cont->particleData()->grid());
}

Post3dWindowZoneDataItem* Post3dWindowParticlesTopDataItem::zoneDataItem() const
{
	return dynamic_cast<Post3dWindowZoneDataItem*> (parent());
}
