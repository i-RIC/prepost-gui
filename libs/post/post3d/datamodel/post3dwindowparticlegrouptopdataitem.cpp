#include "post3dwindowparticlegrouptopdataitem.h"
#include "post3dwindowzonedataitem.h"

#include <guicore/postcontainer/postzonedatacontainer.h>

Post3dWindowParticleGroupTopDataItem::Post3dWindowParticleGroupTopDataItem(const std::string& name, Post3dWindowDataItem* parent) :
	Post3dWindowParticlesBaseTopDataItem {name.c_str(), parent},
	m_name (name)
{}

const std::string& Post3dWindowParticleGroupTopDataItem::name() const
{
	return m_name;
}

vtkPolyData* Post3dWindowParticleGroupTopDataItem::particleData() const
{
	return zoneDataItem()->dataContainer()->particleGroup(m_name);
}

Post3dWindowZoneDataItem* Post3dWindowParticleGroupTopDataItem::zoneDataItem() const
{
	return dynamic_cast<Post3dWindowZoneDataItem*> (parent()->parent());
}
