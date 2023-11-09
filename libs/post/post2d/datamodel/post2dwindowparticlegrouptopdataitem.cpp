#include "post2dwindowcalculationresultdataitem.h"
#include "post2dwindowparticlegrouptopdataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guibase/vtkpointsetextended/vtkpointsetextended.h>
#include <guicore/grid/v4particles2d.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>

#include <vtkPolyData.h>

Post2dWindowParticleGroupTopDataItem::Post2dWindowParticleGroupTopDataItem(const std::string& name, Post2dWindowDataItem* parent) :
	Post2dWindowParticlesBaseTopDataItem {name.c_str(), parent},
	m_name (name)
{}

const std::string& Post2dWindowParticleGroupTopDataItem::name() const
{
	return m_name;
}

v4Particles2d* Post2dWindowParticleGroupTopDataItem::particleData() const
{
	auto cont = zoneDataItem()->v4DataContainer();
	if (cont == nullptr) {return nullptr;}

	return dynamic_cast<v4Particles2d*> (cont->particleGroup(m_name)->grid());
}

Post2dWindowCalculationResultDataItem* Post2dWindowParticleGroupTopDataItem::resultDataItem() const
{
	return dynamic_cast<Post2dWindowCalculationResultDataItem*> (parent()->parent());
}
