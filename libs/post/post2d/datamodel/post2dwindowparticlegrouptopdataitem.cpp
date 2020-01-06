#include "post2dwindowparticlegrouptopdataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guicore/postcontainer/postzonedatacontainer.h>

Post2dWindowParticleGroupTopDataItem::Post2dWindowParticleGroupTopDataItem(const std::string& name, Post2dWindowDataItem* parent) :
	Post2dWindowParticlesBaseTopDataItem {name.c_str(), parent},
	m_name (name)
{}

const std::string& Post2dWindowParticleGroupTopDataItem::name() const
{
	return m_name;
}

vtkPolyData* Post2dWindowParticleGroupTopDataItem::particleData() const
{
	return zoneDataItem()->dataContainer()->particleGroup(m_name);
}

Post2dWindowZoneDataItem* Post2dWindowParticleGroupTopDataItem::zoneDataItem() const
{
	return dynamic_cast<Post2dWindowZoneDataItem*> (parent()->parent());
}
