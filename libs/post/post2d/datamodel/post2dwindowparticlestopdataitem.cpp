#include "post2dwindowcalculationresultdataitem.h"
#include "post2dwindowparticlestopdataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guicore/grid/v4particles2d.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>

Post2dWindowParticlesTopDataItem::Post2dWindowParticlesTopDataItem(Post2dWindowDataItem* p) :
	Post2dWindowParticlesBaseTopDataItem {tr("Particles"), p}
{}

v4Particles2d* Post2dWindowParticlesTopDataItem::particleData() const
{
	auto cont = zoneDataItem()->v4DataContainer();
	if (cont == nullptr) {return nullptr;}

	return dynamic_cast<v4Particles2d*> (cont->particleData()->grid());
}

Post2dWindowCalculationResultDataItem* Post2dWindowParticlesTopDataItem::resultDataItem() const
{
	return dynamic_cast<Post2dWindowCalculationResultDataItem*> (parent());
}
