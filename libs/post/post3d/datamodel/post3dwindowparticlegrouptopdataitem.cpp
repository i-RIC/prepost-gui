#include "post3dwindowparticlegrouptopdataitem.h"
#include "post3dwindowzonedataitem.h"

#include <guicore/grid/v4particles3d.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>

Post3dWindowParticleGroupTopDataItem::Post3dWindowParticleGroupTopDataItem(const std::string& name, Post3dWindowDataItem* parent) :
	Post3dWindowParticlesBaseTopDataItem {name.c_str(), parent},
	m_name (name)
{}

const std::string& Post3dWindowParticleGroupTopDataItem::name() const
{
	return m_name;
}

v4Particles3d* Post3dWindowParticleGroupTopDataItem::particleData() const
{
	auto cont = zoneDataItem()->v4DataContainer();
	if (cont == nullptr) {return nullptr;}

	return dynamic_cast<v4Particles3d*> (cont->particleGroup(m_name)->grid());
}

Post3dWindowZoneDataItem* Post3dWindowParticleGroupTopDataItem::zoneDataItem() const
{
	return dynamic_cast<Post3dWindowZoneDataItem*> (parent()->parent());
}
